/* Xplayer Cone plugin
 *
 * Copyright © 2004 Bastien Nocera <hadess@hadess.net>
 * Copyright © 2002 David A. Schleef <ds@schleef.org>
 * Copyright © 2006, 2008 Christian Persch
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
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 */

#include <config.h>

#include <string.h>

#include <glib.h>

#include "xplayerPlugin.h"
#include "xplayerConeAudio.h"

static const char *propertyNames[] = {
  "channel"
  "mute",
  "track",
  "volume",
};

static const char *methodNames[] = {
  "toggleMute"
};

XPLAYER_IMPLEMENT_NPCLASS (xplayerConeAudio,
                         propertyNames, G_N_ELEMENTS (propertyNames),
                         methodNames, G_N_ELEMENTS (methodNames),
                         NULL);

xplayerConeAudio::xplayerConeAudio (NPP aNPP)
  : xplayerNPObject (aNPP),
    mMute (false),
    mSavedVolume (0.5)
{
  XPLAYER_LOG_CTOR ();
}

xplayerConeAudio::~xplayerConeAudio ()
{
  XPLAYER_LOG_DTOR ();
}

bool
xplayerConeAudio::InvokeByIndex (int aIndex,
                               const NPVariant *argv,
                               uint32_t argc,
                               NPVariant *_result)
{
  XPLAYER_LOG_INVOKE (aIndex, xplayerConeAudio);

  switch (Methods (aIndex)) {
    case eToggleMute: {
      /* FIXMEchpe this sucks */
      NPVariant mute;
      BOOLEAN_TO_NPVARIANT (!mMute, mute);
      return SetPropertyByIndex (eMute, &mute);
    }
  }

  return false;
}

bool
xplayerConeAudio::GetPropertyByIndex (int aIndex,
                                    NPVariant *_result)
{
  XPLAYER_LOG_GETTER (aIndex, xplayerConeAudio);

  switch (Properties (aIndex)) {
    case eMute:
      return BoolVariant (_result, Plugin()->IsMute());

    case eVolume:
      return Int32Variant (_result, Plugin()->Volume() * 200.0);

    case eChannel:
    case eTrack:
      XPLAYER_WARN_GETTER_UNIMPLEMENTED (aIndex, _result);
      return VoidVariant (_result);
  }

  return false;
}

bool
xplayerConeAudio::SetPropertyByIndex (int aIndex,
                                    const NPVariant *aValue)
{
  XPLAYER_LOG_SETTER (aIndex, xplayerConeAudio);

  switch (Properties (aIndex)) {
    case eVolume: {
      int32_t volume;
      if (!GetInt32FromArguments (aValue, 1, 0, volume))
        return false;

      Plugin()->SetVolume ((double) CLAMP (volume, 0, 200) / 200.0);
      return true;
    }

    case eMute: {
      if (!GetBoolFromArguments (aValue, 1, 0, mMute))
        return false;

      if (mMute) {
        mSavedVolume = Plugin()->Volume();
        Plugin()->SetVolume (0.0);
      } else {
        Plugin()->SetVolume (mSavedVolume);
      }
      return true;
    }

    case eChannel:
    case eTrack:
      XPLAYER_WARN_SETTER_UNIMPLEMENTED (aIndex, _result);
      return true;
  }

  return false;
}
