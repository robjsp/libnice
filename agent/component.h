/*
 * This file is part of the Nice GLib ICE library.
 *
 * (C) 2006, 2007 Collabora Ltd.
 *  Contact: Dafydd Harries
 * (C) 2006, 2007 Nokia Corporation. All rights reserved.
 *  Contact: Kai Vehmanen
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Nice GLib ICE library.
 *
 * The Initial Developers of the Original Code are Collabora Ltd and Nokia
 * Corporation. All Rights Reserved.
 *
 * Contributors:
 *   Dafydd Harries, Collabora Ltd.
 *   Kai Vehmanen, Nokia
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * the GNU Lesser General Public License Version 2.1 (the "LGPL"), in which
 * case the provisions of LGPL are applicable instead of those above. If you
 * wish to allow use of your version of this file only under the terms of the
 * LGPL and not to allow others to use your version of this file under the
 * MPL, indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL. If you do
 * not delete the provisions above, a recipient may use your version of this
 * file under either the MPL or the LGPL.
 */

#ifndef _NICE_COMPONENT_H
#define _NICE_COMPONENT_H

#include <glib.h>

#include "agent.h"
#include "candidate.h"

G_BEGIN_DECLS


/* (ICE §4.1.1.1, ID-19) ""For RTP-based media streams, the RTP itself has a component
 * ID of 1, and RTCP a component ID of 2.  If an agent is using RTCP it MUST
 * obtain a candidate for it.  If an agent is using both RTP and RTCP, it
 * would end up with 2*K host candidates if an agent has K interfaces.""
 */

typedef struct _Component Component;
typedef struct _CandidatePair CandidatePair;
typedef struct _IncomingCheck IncomingCheck;

struct _CandidatePair
{
  NiceCandidate *local;
  NiceCandidate *remote;
  guint64 priority;           /**< candidate pair priority */  
};

struct _IncomingCheck
{
  NiceAddress from;
  NiceUDPSocket *local_socket;
  uint32_t priority;
  gboolean use_candidate;
};

struct _Component
{
  NiceComponentType type;
  guint id;                    /**< component id */
  NiceComponentState state;
  GSList *local_candidates;    /**< list of Candidate objs */
  GSList *remote_candidates;   /**< list of Candidate objs */
  GSList *sockets;             /**< list of NiceUDPSocket objs */
  GSList *gsources;            /**< list of GSource objs */
  GSList *incoming_checks;     /**< list of IncomingCheck objs */
  CandidatePair selected_pair; /**< independent from checklists, 
				    see ICE 11.1. "Sending Media" (ID-19) */
  gboolean media_after_tick;   /**< true if media received since last
				    keepalive tick */
  NiceCandidate *restart_candidate; /**< for storing active remote candidate during a restart */
  NiceAgentRecvFunc g_source_io_cb; /**< function called on io cb */
  gpointer data;                    /**< data passed to the io function */
  GMainContext *ctx;                /**< context for data callbacks for this
                                       component */
};

Component *
component_new (
  G_GNUC_UNUSED
  guint component_id);

void
component_free (Component *cmp);

NiceUDPSocket *
component_find_udp_socket_by_fd (Component *component, guint fd);

gboolean
component_find_pair (Component *cmp, NiceAgent *agent, const gchar *lfoundation, const gchar *rfoundation, CandidatePair *pair);

gboolean
component_restart (Component *cmp);

void
component_update_selected_pair (Component *component, const CandidatePair *pair);

NiceCandidate *
component_find_remote_candidate (const Component *component, const NiceAddress *addr, NiceCandidateTransport transport);

G_END_DECLS

#endif /* _NICE_COMPONENT_H */

