#include "main.h"

int main(int argc, char *argv[])
{
	GtkWidget *main_window;

    gtk_init(&argc, &argv);

	// make main window and connect destroy signal
	main_window = make_window();
	g_signal_connect_swapped(G_OBJECT(main_window), "destroy",
			G_CALLBACK(gtk_main_quit), NULL);

    // fire it up!
    gtk_main();

    return 0;
}

gboolean window_key_press(GtkWidget *window, GdkEventKey *event,
        struct Window *win_data)
{
    if (event->state == 5)  // CTRL+SHIFT was pressed
    {
        switch (event->keyval)
        {
            case 'C':
                vte_terminal_copy_clipboard(
                        VTE_TERMINAL(win_data->terminal));
                return TRUE;
                break;
            case 'V': 
                vte_terminal_paste_clipboard(
                        VTE_TERMINAL(win_data->terminal));
                return TRUE;
                break;
        }
    }
    return FALSE;
}

GtkWidget *make_window()
{
	GtkWidget *window;
    gchar **term_argv;
    term_argv = g_strsplit_set("", ",", 1);
    struct Window *win_data = g_new0(struct Window, 1);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color_bg);

    // create new vte widget and set a bunch of settings
	win_data->terminal = vte_terminal_new();
    vte_terminal_set_font_from_string(
            VTE_TERMINAL(win_data->terminal), "terminus 16");
    vte_terminal_set_scrollback_lines(
            VTE_TERMINAL(win_data->terminal), 100000);
    vte_terminal_set_colors(VTE_TERMINAL(win_data->terminal),
            &color_fg, &color_bg,
            color_theme, THEME_COLORS);

    // kill app on terminal exit
    g_signal_connect_swapped(
            G_OBJECT(win_data->terminal),
            "child_exited",
            G_CALLBACK(gtk_main_quit),
            NULL);

    // fire up child process in terminal widget
	term_argv[0] = "/bin/bash";
	if (!vte_terminal_fork_command_full(
			VTE_TERMINAL(win_data->terminal), VTE_PTY_DEFAULT,
			NULL, term_argv, NULL, 0, NULL, NULL,
			NULL, NULL))
	{
		printf("Unable to fork VTE terminal!  Exiting!\n");
		exit(1);
	}

    // keypress magic (copy/paste)
    g_signal_connect(G_OBJECT(window), "key-press-event",
            G_CALLBACK(window_key_press), win_data);

    // glue everything together
	gtk_container_add(GTK_CONTAINER(window), win_data->terminal);
	gtk_widget_show(win_data->terminal);
	gtk_widget_show(window);

	return window;
}


