#include "giodbus.h"
#include <gio/gio.h>
#include <gio-unix-2.0/gio/gunixfdlist.h>
#include <glib.h>

int get_server_gvariant_stdout (int drvid)
{
    GDBusMessage *method_call_message;
    GDBusMessage *method_reply_message;
    GUnixFDList *fd_list;
    GError          **error = NULL;

    gint fd,dup_fd;
    const gchar * response;
    fd = -1;
    dup_fd = -1;
    method_call_message = NULL;
    method_reply_message = NULL;
    GDBusConnection *con = g_bus_get_sync(G_BUS_TYPE_SYSTEM,NULL,NULL);
    method_call_message = g_dbus_message_new_method_call ("org.ukui.Biometric",
                                                          "/org/ukui/Biometric",
                                                          "org.ukui.Biometric",
                                                          "GetFrameFd");

    g_dbus_message_set_body (method_call_message, g_variant_new ("(i)", drvid));
    method_reply_message = g_dbus_connection_send_message_with_reply_sync (con,
                                                                           method_call_message,
                                                                           G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                                           -1,
                                                                           NULL, /* out_serial */
                                                                           NULL, /* cancellable */
                                                                           error);
    if (method_reply_message == NULL)
        goto out;

    if (g_dbus_message_get_message_type (method_reply_message) == G_DBUS_MESSAGE_TYPE_ERROR)
    {
        g_dbus_message_to_gerror (method_reply_message, error);
        goto out;
    }

    fd_list = g_dbus_message_get_unix_fd_list(method_reply_message);
    fd = g_unix_fd_list_get(fd_list,0,error);
     dup_fd = dup(fd);


out:
    g_object_unref (method_call_message);
    g_object_unref (method_reply_message);
    return dup_fd;

}
