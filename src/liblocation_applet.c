/*
 * Hildon Control Panel Location applet/plugin
 * Copyright (c) 2020 Ivan J. <parazyd@dyne.org>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include <libintl.h>

#include <hildon/hildon.h>
#include <hildon-cp-plugin/hildon-cp-plugin-interface.h>

#define GCONF_KEY_GPS_DISABLED    "/system/nokia/location/gps-disabled"
#define GCONF_KEY_NET_DISABLED    "/system/nokia/location/network-disabled"
#define GCONF_KEY_SUPL_SERVER     "/system/nokia/location/supl/server"
#define GCONF_KEY_LOCATION_METHOD "/system/nokia/location/method"

typedef struct {
	osso_context_t *osso;
	HildonWindow *parent;
	gboolean state;
	GtkWidget *dialog;
	GtkWidget *pan;
	GtkWidget *box;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *gps_label;
	GtkWidget *gps_check_btn;
	GtkWidget *gps_device_btn;
	GtkWidget *gps_pair_btn;
	GtkWidget *net_label;
	GtkWidget *net_check_btn;
	GtkWidget *server_label;
	GtkWidget *supl_server_entry;
} cpa_dialog;

static gboolean cpa_save_settings(cpa_dialog *dialog)
{
	GConfClient *gconf_client = gconf_client_get_default();
	g_assert(GCONF_IS_CLIENT(gconf_client));

	gconf_client_set_bool(gconf_client,
			GCONF_KEY_GPS_DISABLED,
			!hildon_check_button_get_active(HILDON_CHECK_BUTTON(dialog->gps_check_btn)),
			NULL);

	gconf_client_set_bool(gconf_client,
			GCONF_KEY_NET_DISABLED,
			!hildon_check_button_get_active(HILDON_CHECK_BUTTON(dialog->net_check_btn)),
			NULL);

	gconf_client_set_string(gconf_client,
			GCONF_KEY_SUPL_SERVER,
			hildon_entry_get_text(HILDON_ENTRY(dialog->supl_server_entry)),
			NULL);

	return TRUE;
}

static void cpa_hide_ui(cpa_dialog *dialog)
{
	gtk_widget_hide(dialog->supl_server_entry);
	gtk_widget_hide(dialog->server_label);
	gtk_widget_hide(dialog->net_check_btn);
	gtk_widget_hide(dialog->net_label);
	gtk_widget_hide(dialog->gps_pair_btn);
	gtk_widget_hide(dialog->gps_device_btn);
	gtk_widget_hide(dialog->gps_check_btn);
	gtk_widget_hide(dialog->gps_label);
	gtk_widget_hide(dialog->vbox);
	gtk_widget_hide(dialog->hbox);
	gtk_widget_hide(dialog->box);
	gtk_widget_hide(dialog->pan);
	gtk_widget_hide(dialog->dialog);
	gtk_widget_queue_resize(dialog->dialog);
}

static void _dialog_response_cb(GtkDialog *_dialog, gint response_id, gpointer user_data)
{
	cpa_dialog *dialog = user_data;
	GtkWindow *transient;

	if (response_id == GTK_RESPONSE_APPLY) {
		cpa_save_settings(dialog);
	} else if (response_id != GTK_RESPONSE_DELETE_EVENT) {
		g_debug("Wrong response ID! Some error has occured");
		return;
	}

	cpa_hide_ui(dialog);
	transient = gtk_window_get_transient_for(GTK_WINDOW(dialog->dialog));
	if (transient)
		gtk_widget_set_sensitive(GTK_WIDGET(transient), TRUE);

	gtk_widget_destroy(GTK_WIDGET(dialog->dialog));
	gtk_main_quit();
}

static void _gps_check_btn_toggled_cb(GtkDialog *_dialog, gpointer user_data)
{
	cpa_dialog *dialog = user_data;

	if (hildon_check_button_get_active(HILDON_CHECK_BUTTON(dialog->gps_check_btn))) {
		gtk_widget_set_sensitive(dialog->gps_device_btn, TRUE);
		gtk_widget_set_sensitive(dialog->gps_pair_btn, TRUE);
	} else {
		gtk_widget_set_sensitive(dialog->gps_device_btn, FALSE);
		gtk_widget_set_sensitive(dialog->gps_pair_btn, FALSE);
	}
}

static void _net_check_btn_toggled_cb(GtkDialog *_dialog, gpointer user_data)
{
	cpa_dialog *dialog = user_data;

	if (hildon_check_button_get_active(HILDON_CHECK_BUTTON(dialog->net_check_btn))) {
		gtk_widget_set_sensitive(dialog->server_label, TRUE);
		gtk_widget_set_sensitive(dialog->supl_server_entry, TRUE);
	} else {
		gtk_widget_set_sensitive(dialog->server_label, FALSE);
		gtk_widget_set_sensitive(dialog->supl_server_entry, FALSE);
	}
}

static cpa_dialog *cpa_dialog_new(GtkWindow *parent)
{
	cpa_dialog *dialog;
	GtkWidget *selector;
	GtkListStore *liststore;
	GtkTreeModel *model;
	GtkTreeIter iter;

	GConfClient *gconf_client = gconf_client_get_default();
	g_assert(GCONF_IS_CLIENT(gconf_client));

	gboolean gps_disabled = gconf_client_get_bool(gconf_client, GCONF_KEY_GPS_DISABLED, NULL);
	gboolean net_disabled = gconf_client_get_bool(gconf_client, GCONF_KEY_NET_DISABLED, NULL);

	dialog = g_try_new0(cpa_dialog, 1);
	dialog->pan = hildon_pannable_area_new();
	dialog->gps_label = gtk_label_new(dgettext("osso-location-ui", "loca_ti_general"));
	dialog->gps_check_btn = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);

	gtk_button_set_label(GTK_BUTTON(dialog->gps_check_btn),
			dgettext("osso-location-ui", "loca_fi_gps_on"));
	hildon_check_button_set_active(
			HILDON_CHECK_BUTTON(dialog->gps_check_btn), !gps_disabled);

	/* TODO: Work this list out so it's functional */
	liststore = gtk_list_store_new(3, 64, 64, 64);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0,
			dgettext("osso-location-ui", "loca_li_device_internal"),
			1, "las", 2, "com.nokia.Location", -1);

	model = GTK_TREE_MODEL(liststore);
	selector = hildon_touch_selector_new();
	hildon_touch_selector_append_text_column(
			HILDON_TOUCH_SELECTOR(selector), model, FALSE);
	/* g_object_set(selector, "text-column"); */

	dialog->gps_device_btn = hildon_picker_button_new(HILDON_SIZE_FINGER_HEIGHT,
			HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
	hildon_button_set_title(HILDON_BUTTON(dialog->gps_device_btn),
			dgettext("osso-location-ui", "loca_fi_device"));

	hildon_picker_button_set_selector(
			HILDON_PICKER_BUTTON(dialog->gps_device_btn),
			HILDON_TOUCH_SELECTOR(selector));

	/* TODO: ^^ */

	hildon_button_set_alignment(HILDON_BUTTON(dialog->gps_device_btn),
			0.0, 0.0, 0.5, 1.0);
	hildon_button_set_title_alignment(HILDON_BUTTON(dialog->gps_device_btn),
			0.0, 0.5);
	hildon_button_set_value_alignment(HILDON_BUTTON(dialog->gps_device_btn),
			1.0, 0.5);

	/* TODO: Test this pairing functionality on Fremantle */
	dialog->gps_pair_btn = hildon_button_new_with_text(
			HILDON_SIZE_FINGER_HEIGHT,
			HILDON_BUTTON_ARRANGEMENT_HORIZONTAL,
			dgettext("osso-location-ui", "loca_bv_pair_new_device"),
			NULL);
	hildon_button_set_alignment(HILDON_BUTTON(dialog->gps_pair_btn),
			0.0, 0.0, 1.0, 1.0);

	dialog->net_label = gtk_label_new(
			dgettext("osso-location-ui", "loca_fi_network"));

	dialog->net_check_btn = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);

	gtk_button_set_label(GTK_BUTTON(dialog->net_check_btn),
			dgettext("osso-location-ui", "loca_fi_enable_network_posit"));
	hildon_check_button_set_active(
			HILDON_CHECK_BUTTON(dialog->net_check_btn), !net_disabled);
	/* XXX: Disabled until actual network positioning is implemented.
	 * Note that liblocation and/or location-daemon also need to become
	 * aware if this and act accordingly.
	 * e.g. Don't start the gpsd part if GPS is disabled but network is
	 */
	gtk_widget_set_sensitive(dialog->net_check_btn, FALSE);

	dialog->server_label = gtk_label_new(
			dgettext("osso-location-ui", "loca_fi_location_server"));
	dialog->supl_server_entry = hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT);
	hildon_entry_set_text(HILDON_ENTRY(dialog->supl_server_entry),
			gconf_client_get_string(gconf_client, GCONF_KEY_SUPL_SERVER, NULL));

	dialog->box = gtk_vbox_new(TRUE, 0);
	dialog->hbox = gtk_hbox_new(FALSE, 0);
	dialog->vbox = gtk_vbox_new(TRUE, 0);

	gtk_widget_set_sensitive(dialog->gps_device_btn, !gps_disabled);
	gtk_widget_set_sensitive(dialog->gps_pair_btn, !gps_disabled);
	gtk_widget_set_sensitive(dialog->server_label, !net_disabled);
	gtk_widget_set_sensitive(dialog->supl_server_entry, !net_disabled);

	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->gps_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->gps_check_btn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->gps_device_btn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->gps_pair_btn, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->net_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->net_check_btn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->hbox), dialog->server_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->hbox), dialog->supl_server_entry, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->hbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->box), dialog->vbox, FALSE, FALSE, 0);

	dialog->dialog = gtk_dialog_new_with_buttons(
			dgettext("osso-location-ui", "loca_ti_location_cpa"),
			GTK_WINDOW(parent),
			GTK_DIALOG_NO_SEPARATOR|GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL,
			NULL);

	hildon_pannable_area_add_with_viewport(
			HILDON_PANNABLE_AREA(dialog->pan), dialog->box);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog->dialog)->vbox), dialog->pan,
			TRUE, TRUE, 0);
	gtk_dialog_add_buttons(GTK_DIALOG(dialog->dialog),
			dgettext("hildon-libs", "wdgt_bd_save"),
			GTK_RESPONSE_APPLY, NULL);

	g_signal_connect(dialog->gps_check_btn, "toggled",
			G_CALLBACK(_gps_check_btn_toggled_cb), dialog);
	g_signal_connect(dialog->net_check_btn, "toggled",
			G_CALLBACK(_net_check_btn_toggled_cb), dialog);
	g_signal_connect(dialog->dialog, "response",
			G_CALLBACK(_dialog_response_cb), dialog);

	return dialog;
}

static gboolean cpa_state_load(gboolean *state_data, osso_context_t *osso)
{
	osso_state_t state;

	state.state_size = sizeof(*state_data);
	state.state_data = state_data;

	return osso_state_read(osso, &state) == OSSO_OK;
}

static gboolean cpa_update_ui(cpa_dialog *dialog)
{
	return TRUE;
}

static void cpa_unhide_hide_ui(cpa_dialog *dialog)
{
	gtk_widget_show_all(dialog->pan);
	gtk_widget_set_size_request(dialog->pan, -1, 350);
	gtk_widget_show(dialog->dialog);
	gtk_widget_queue_resize(dialog->dialog);
}

static gint cpa_dialog_run(cpa_dialog *dialog)
{
	cpa_update_ui(dialog);

	if (!cpa_state_load(&dialog->state, dialog->osso))
		dialog->state = 0;

	if (dialog->state == 0) {
		cpa_unhide_hide_ui(dialog);
		return gtk_dialog_run(GTK_DIALOG(dialog->dialog));
	} else {
		return 1;
	}
}

static gboolean _idle_present_parent_window_cb(gpointer user_data)
{
	cpa_dialog *dialog = user_data;

	if (dialog) {
		cpa_dialog_run(dialog);
		gtk_window_present(GTK_WINDOW(dialog->parent));
	}

	return FALSE;
}

osso_return_t execute(osso_context_t *osso, gpointer data, gboolean user_activated)
{
	GtkWindow *parent = (GtkWindow *)data;
	osso_return_t rv = OSSO_ERROR;

	if (parent) {
		cpa_dialog *dialog = cpa_dialog_new(parent);

		if (dialog) {
			dialog->parent = HILDON_WINDOW(parent);
			g_idle_add(_idle_present_parent_window_cb, dialog);
			gtk_main();
			rv = OSSO_OK;
		} else {
			g_critical("Unabel to create applet dialog. Exiting.");
		}
	} else {
		g_critical("Missing data.");
	}

	return rv;
}

osso_return_t save_state(osso_context_t *osso, gpointer data)
{
	return OSSO_OK;
}
