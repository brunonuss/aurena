/* GStreamer
 * Copyright (C) 2012-2014 Jan Schmidt <thaytan@noraisin.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef __AUR_CLIENT_H__
#define __AUR_CLIENT_H__

#include <gst/gst.h>
#include <gst/net/gstnet.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

#if HAVE_AVAHI
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-glib/glib-watch.h>
#endif

#include "src/common/aur-types.h"

G_BEGIN_DECLS

#define AUR_TYPE_CLIENT (aur_client_get_type ())

typedef struct _AurClientClass AurClientClass;

typedef struct
{
  guint id;
  gchar * host;
  gdouble volume;
  gboolean enabled;
} AurPlayerInfo;

typedef enum
{
    AUR_CLIENT_NONE = 0,
    AUR_CLIENT_PLAYER = (1 << 0),
    AUR_CLIENT_CONTROLLER = (1 << 1),
} AurClientFlags;

struct _AurClient
{
  GObject parent;

  AurClientFlags flags;
  gdouble volume;
  GArray *player_info;

  gboolean enabled;
  gboolean paused;
  GstClockTime base_time;
  GstClockTime position;
  gchar *uri;
  gchar *language;

  GstClock *net_clock;
  gchar *server_host;
  gint server_port;

  SoupSession *soup;
  SoupMessage *msg;
  JsonParser *json;

  GMainContext * context;

  GstElement *player;
  GSource *bus_source;

  guint timeout;
  guint idle_timeout;

  gboolean connecting;
  gboolean was_connected;
  gboolean shutting_down;
  gchar *connected_server;
  gint connected_port;

#if HAVE_AVAHI
  AvahiGLibPoll *glib_poll;
  AvahiClient *avahi_client;
  AvahiServiceBrowser *avahi_sb;
#endif
};

struct _AurClientClass
{
  GObjectClass parent;
};

GType aur_client_get_type(void);
AurClient *aur_client_new(GMainContext * context, const gchar *server, AurClientFlags flags);

gboolean aur_client_is_connected (AurClient * client);
gboolean aur_client_is_enabled (AurClient * client);
gboolean aur_client_is_playing (AurClient * client);
void aur_client_set_media (AurClient * client, const gchar * id);
void aur_client_next (AurClient * client, guint id);
void aur_client_play (AurClient * client);
void aur_client_pause (AurClient * client);
void aur_client_seek (AurClient * client, GstClockTime position);
void aur_client_get_volume (AurClient * client, gdouble volume);
void aur_client_set_volume (AurClient * client, gdouble volume);
const GArray *aur_client_get_player_info (AurClient * client);
gboolean aur_client_get_player_enabled (AurClient * client, guint id);
void aur_client_set_player_enabled (AurClient * client, guint id, gboolean enabled);
void aur_client_set_player_volume (AurClient * client, guint id, gdouble volume);
void aur_client_set_language (AurClient * client, const gchar *language_code);

G_END_DECLS
#endif
