/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef __HILDON_INPUT_MODE_HINT_H__
#define __HILDON_INPUT_MODE_HINT_H__

G_BEGIN_DECLS

/* Hildon wrapper for setting the input mode in a GtkEntry 
 * Usage: g_object_set(G_OBJECT(entry), HILDON_INPUT_MODE_HINT, HILDON_INPUT_MODE_HINT_HEXA, NULL);
 */
#define HILDON_INPUT_MODE_HINT "input-mode"

/* Hildon wrapper for setting the autocapitalization in text widgets.
 * Usage: g_object_set(G_OBJECT(entry), HILDON_AUTOCAP, FALSE, NULL);
 */
#define HILDON_AUTOCAP "autocap"

/**
 * HildonInputModeHint:
 * @HILDON_INPUT_MODE_HINT_ALPHANUMERICSPECIAL: accept all characters.
 * @HILDON_INPUT_MODE_HINT_NUMERIC: accept only NUMERIC characters.
 * @HILDON_INPUT_MODE_HINT_ALPHA: accept only ALPHA characters
 * @HILDON_INPUT_MODE_HINT_NUMERICSPECIAL: accept only NUMERIC and SPECIAL 
 * @HILDON_INPUT_MODE_HINT_ALPHASPECIAL: accept only ALPHA and SPECIAL 
 * @HILDON_INPUT_MODE_HINT_ALPHANUMERIC: accept only ALPHA and NUMERIC
 * @HILDON_INPUT_MODE_HINT_HEXA: accept only HEXA 
 * @HILDON_INPUT_MODE_HINT_HEXASPECIAL: accept only HEXA and SPECIAL
 * @HILDON_INPUT_MODE_HINT_TELE: accept only TELEPHONE
 * @HILDON_INPUT_MODE_HINT_TELESPECIAL: accept only TELEPHONE and SPECIAL
 *
 * Keys to set the mode in a GtkEntry widget into ALPHANUMERIC or NUMERIC mode. Note that this is only a hint; it only shows VKB with specified layout. Use it by calling 'g_object_set(G_OBJECT(entry), "input-mode", HILDON_INPUT_MODE_HINT_NUMERIC, NULL);'.
 */
typedef enum {
  HILDON_INPUT_MODE_HINT_ALPHANUMERICSPECIAL = 0,
  HILDON_INPUT_MODE_HINT_NUMERIC,
  HILDON_INPUT_MODE_HINT_ALPHA,
  HILDON_INPUT_MODE_HINT_NUMERICSPECIAL,
  HILDON_INPUT_MODE_HINT_ALPHASPECIAL,
  HILDON_INPUT_MODE_HINT_ALPHANUMERIC,
  HILDON_INPUT_MODE_HINT_HEXA,
  HILDON_INPUT_MODE_HINT_HEXASPECIAL,
  HILDON_INPUT_MODE_HINT_TELE,
  HILDON_INPUT_MODE_HINT_TELESPECIAL

} HildonInputModeHint;

G_END_DECLS
#endif /* __HILDON_INPUT_MODE_HINT_H__ */
