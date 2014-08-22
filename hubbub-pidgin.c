/*
 * Hubbub-Pidgin Plugin
 *
 * Copyright (C) 2014, Hugo Herter <hugoherter.com>
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* config.h may define PURPLE_PLUGINS; protect the definition here so that we
 * don't get complaints about redefinition when it's not necessary. */
#ifndef PURPLE_PLUGINS
# define PURPLE_PLUGINS
#endif

#include <glib.h>

/* This will prevent compiler errors in some instances and is better explained in the
 * how-to documents on the wiki */
#ifndef G_GNUC_NULL_TERMINATED
# if __GNUC__ >= 4
#  define G_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
# else
#  define G_GNUC_NULL_TERMINATED
# endif
#endif

#include "debug.h"
#include <notify.h>
#include <plugin.h>
#include <version.h>

#include <string.h> // for strncmp

#include "hubbub-pidgin-utils.c"

// For keeping a reference to the fork starting the Hubbub engine:
int hubbub_engine_status;

/* we're adding this here and assigning it in plugin_load because we need
 * a valid plugin handle for our call to purple_notify_message() in the
 * plugin_action_test_cb() callback function */
PurplePlugin *helloworld_plugin = NULL;

/* This function is the callback for the plugin action we added. All we're
 * doing here is displaying a message. When the user selects the plugin
 * action, this function is called. */
static void
plugin_action_control_panel (PurplePluginAction * action)
{
    /*
    purple_notify_message (helloworld_plugin, PURPLE_NOTIFY_MSG_INFO,
        "Plugin Actions Test", "This is a plugin actions test :)", NULL, NULL,
        NULL);
    */
    if ( system("xdg-open http://127.0.0.1:8900") > 0)
    {
        purple_debug_info( 
            "core-hoh-hubbub-pidgin",
            "Error while launching browser.\n"
            );
    }
}

/**
 * This is our callback for the receiving-im-msg signal.
 *
 * We return TRUE to block the IM, FALSE to accept the IM
 */
static gboolean
receiving_im_msg_cb(
    PurpleAccount * account,
    char **sender,
    char **buffer,
    PurpleConversation * conv,
    PurpleMessageFlags * flags,
    void *data)
{
    purple_debug_info("core-hoh-hubbub-pidgin",
                "Called receiving_im_msg_cb.\n");

    printf("-------\n");
    //printf(g_strdup(*buffer));
    printf("\n<-------\n");

    log_dummy_message();

    if (strncmp(*buffer, "?OTR:", 3) == 0)
        printf("This shouldn't be OTR-encrypted.\n");
    else
        printf("This looks unencrypted.\n");

    if ((strncmp(*buffer, "?DUMMY:", 7) == 0) ||
	(strncmp(*buffer, "<FONT>?DUMMY:", 13) == 0))
    {
        printf("This is a dummy message..\n");
        return TRUE;              /* returning TRUE will block the IM */
    }
    else {
        printf("This looks like a real message.\n");
        return FALSE;              /* returning TRUE will block the IM */
    }

}

static gboolean
writing_im_msg_cb(
    PurpleAccount *account,
    const char *who,
    char **message,
    PurpleConversation *conv,
    PurpleMessageFlags flags)
{
    purple_debug_info("core-hoh-hubbub-pidgin",
                "Called writing_im_msg_cb.\n");

    if ((strncmp(*message, "?DUMMY:", 7) == 0) || 
	(strncmp(*message, "<FONT>?DUMMY:", 13) == 0))
        return TRUE;
    else
        return FALSE;
}

/* we tell libpurple in the PurplePluginInfo struct to call this function to
 * get a list of plugin actions to use for the plugin.  This function gives
 * libpurple that list of actions. */
static GList *
plugin_actions (PurplePlugin * plugin, gpointer context)
{
    /* some C89 (a.k.a. ANSI C) compilers will warn if any variable declaration
     * includes an initilization that calls a function.  To avoid that, we
     * generally initialize our variables first with constant values like NULL
     * or 0 and assign to them with function calls later */
    GList *list = NULL;
    PurplePluginAction *action_control_panel = NULL;

    /* The action gets created by specifying a name to show in the UI and a
     * callback function to call. */
    action_control_panel = purple_plugin_action_new ("Control Panel", plugin_action_control_panel);

    /* libpurple requires a GList of plugin actions, even if there is only one
     * action in the list.  We append the action to a GList here. */
    list = g_list_append (list, action_control_panel);

    /* Once the list is complete, we send it to libpurple. */
    return list;
}

static gboolean
plugin_load (PurplePlugin * plugin)
{
    printf("Foobar");
    purple_debug_info("core-hoh-hubbub-pidgin", "Fork debug\n");
    
    /*
    purple_notify_message (plugin, PURPLE_NOTIFY_MSG_INFO, "Hello World!",
        "Welcome to the Hubbub-Pidgin plugin ! Make sure the OTR plugin is loaded after me.", NULL, NULL,
        NULL);
    */

    purple_signal_connect(
                purple_conversations_get_handle(),
                "receiving-im-msg",
                plugin,
                PURPLE_CALLBACK(receiving_im_msg_cb),
                NULL);

    purple_signal_connect(
                purple_conversations_get_handle(),
                "writing-im-msg",
                plugin,
                PURPLE_CALLBACK(writing_im_msg_cb),
                NULL);

    helloworld_plugin = plugin; /* assign this here so we have a valid handle later */

    //hubbub_engine_status = system("/home/okso/Documents/Thesis/Hubbub/venv3/bin/python3 -m hubbub fork");
    purple_debug_info("core-hoh-hubbub-pidgin",
                      "Fork finished launching Python.\n");

    return TRUE;
}

/* For specific notes on the meanings of each of these members, consult the C Plugin Howto
 * on the website. */
static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    "core-hoh-hubbub-pidgin",
    "Hubbub",
    DISPLAY_VERSION, /* This constant is defined in config.h, but you shouldn't use it for
            your own plugins.  We use it here because it's our plugin. And we're lazy. */

    "Plugin that hides dummy messages from Hubbub.",
    "Plugin that hides dummy messages used by Hubbub to add noise to your communication patterns. "
    "This plugin will hide from the user any message starting with '?DUMMY:'.",
    "Hubbub Project - Hugo Herter", /* correct author */
    "https://github.com/hoh/Hubbub-Pidgin",


    plugin_load,
    NULL,
    NULL,

    NULL,
    NULL,
    NULL,
    plugin_actions,		/* this tells libpurple the address of the function to call
                   to get the list of plugin actions. */
    NULL,
    NULL,
    NULL,
    NULL
};

static void
init_plugin (PurplePlugin * plugin)
{
}

PURPLE_INIT_PLUGIN (hello_world, init_plugin, info)
