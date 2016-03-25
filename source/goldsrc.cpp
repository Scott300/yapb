//
// Yet Another POD-Bot, based on PODBot by Markus Klinge ("CountFloyd").
// Copyright (c) YaPB Development Team.
//
// This software is licensed under the BSD-style license.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     http://yapb.jeefo.net/license
//

#include <core.h>

enginefuncs_t g_engfuncs;

int Entity::entindex ()
{
   return engine.IndexOfEntity (edict ());
}

bool Entity::IsValid (void)
{
   return (this && pev && (engine.IsNullEntity (edict ()) ? false : (edict ()->free == 0)));
}


const char * Entity::GetClassname (void)
{
   return ValveString::Get (pev->classname);
}

const char * Entity::GetNetName (void)
{
   return (char *) ValveString::Get (pev->netname);
}

const char * Entity::GetModel (void)
{
   if (IsPlayer ())
      return g_engfuncs.pfnInfoKeyValue (g_engfuncs.pfnGetInfoKeyBuffer (edict ()), "model");
   return ValveString::Get (pev->model);
}

const char * Entity::GetViewModel (void)
{
   return ValveString::Get (pev->viewmodel);
}

const char * Entity::GetTargetName (void)
{
   return ValveString::Get (pev->targetname);
}

const char * Entity::GetTarget (void)
{
   return ValveString::Get (pev->target);
}

//=========================================================
// FInViewCone - returns true is the passed vector is in
// the caller's forward view cone.
//=========================================================
bool Entity::FInViewCone (Vector *pOrigin, float fov)
{
   return false;
 /*  if (fov <= 0)
      fov = GetFov ();

   Vector forward;
   GetViewAngles ().AngleVectors (&forward);

   Vector vecLOS = (*pOrigin - GetGunPosition ()).Normalize ();
   float flDot = DotProduct (vecLOS, forward);

   return (flDot >= cos ((fov / 2) * (M_PI / 180)));*/
}

bool Entity::FInViewCone (Entity *pEntity, float fov)
{
   Vector v_origin = pEntity->GetOrigin ();
   return FInViewCone (&v_origin, fov);
}

//=========================================================
// FVisible - returns true if a line can be traced from
// the caller's eyes to the target
//=========================================================
bool Entity::FVisible (const Vector &vecDest)
{
   // don't look through water
  // if (IsLiquid (GetGunPosition ()) != IsLiquid (vecDest))
      return false;

   // check if line of sight to object is not blocked (i.e. visible)
   //return (TestLine (EyePosition (), vecDest).fraction >= 1.0);
}

bool Entity::FVisible (Entity *pEntity)
{
   // don't look through water
  // if (IsInWater () != pEntity->IsInWater ())
      return false;

  // return (TestLine (EyePosition (), pEntity->EyePosition (), true, pEntity).fraction >= 1.0);
}

bool Entity::FBoxVisible (Entity *pEntity, Vector *pvHit, unsigned char *ucBodyPart)
{
   if (ucBodyPart)
      *ucBodyPart = 0;

   return false; // it's invisible
}

void Entity::ChangeAngles (const Vector &idealangles, float speed)
{
   pev->v_angle = pev->angles = idealangles;
   pev->v_angle.x *= -1;
   pev->angles.x /= 3;
}

#define MAX_ENT_INSTANCE 10

Entity *Entity::Instance (edict_t *pent)
{
   static Entity entity[MAX_ENT_INSTANCE];
   static int iCur = 0;
/*
   CEntity *pEntity = CClient::Instance (pent);
   if (pEntity != NULL)
      return pEntity;
*/
   int index = iCur;

   iCur++;
   if (iCur >= MAX_ENT_INSTANCE)
      iCur = 0;

   entity[index].pev = &pent->v;
   return &entity[index];
}



GoldSrcLoader gs;

SHARED_LIBRARAY_EXPORT int GetEntityAPI2 (gamefuncs_t *table, int *)
{
   // this function is called right after FuncPointers_t() by the engine in the game DLL (or
   // what it BELIEVES to be the game DLL), in order to copy the list of MOD functions that can
   // be called by the engine, into a memory block pointed to by the functionTable pointer
   // that is passed into this function (explanation comes straight from botman). This allows
   // the Half-Life engine to call these MOD DLL functions when it needs to spawn an entity,
   // connect or disconnect a player, call Think() functions, Touch() functions, or Use()
   // functions, etc. The bot DLL passes its OWN list of these functions back to the Half-Life
   // engine, and then calls the MOD DLL's version of GetEntityAPI to get the REAL gamedll
   // functions this time (to use in the bot code).

   gs.ExecuteEntityAPIs (table);

   table->pfnGameInit = [] (void)
   {
      gs->OnGameInit ();

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      gs.GetDllapiFunctionTable ()->pfnGameInit ();
   };

   table->pfnSpawn = [] (edict_t *ent)
   {
      gs->OnEntitySpawned (ent);

      if (strcmp (ValveString::Get (ent->v.classname), "player_weaponstrip") == 0)
      {
         if ((g_gameFlags & GAME_LEGACY) && (ValveString::Get (ent->v.target))[0] == '0')
            ent->v.target = ent->v.targetname = ValveString::Make ("CRASHFIX");
         else
         {
            g_engfuncs.pfnRemoveEntity (ent);
            gs->OnGameInit ();

            if (gs.IsMetamod ())
            {
               gs.SetMetaResult (MRES_SUPERCEDE);
               return 0;
            }
            return 0;
         }
      }

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return 0;
      }
      int result = gs.GetDllapiFunctionTable  ()->pfnSpawn (ent); // get result

      if (ent->v.rendermode == kRenderTransTexture)
         ent->v.flags &= ~FL_WORLDBRUSH; // clear the FL_WORLDBRUSH flag out of transparent ents

      return result;
   };

   table->pfnTouch = []  (edict_t *ent1, edict_t *ent2)
   {
      gs->OnEntitiesTouch (ent1, ent2);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      gs.GetDllapiFunctionTable ()->pfnTouch (ent1, ent2);
   };

   table->pfnClientConnect = [] (edict_t *ent, const char *name, const char *addr, char rejectReason[128])
   {
      gs->OnClientConnected (ent, addr);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return 0;
      }
      return gs.GetDllapiFunctionTable ()->pfnClientConnect (ent, name, addr, rejectReason);
   };

   table->pfnClientDisconnect = [] (edict_t *ent)
   {
      gs->OnClientDisconnect (ent);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      gs.GetDllapiFunctionTable ()->pfnClientDisconnect (ent);
   };

   table->pfnClientUserInfoChanged = [] (edict_t *ent, char *infobuffer)
   {
      gs->OnClientUserInfoChanged (ent, infobuffer);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      gs.GetDllapiFunctionTable ()->pfnClientUserInfoChanged (ent, infobuffer);
   };

   table->pfnClientCommand = [] (edict_t *ent)
   {
      const char *args[7];

      for (int i = 0; i < 7; i++)
         args[i] = g_engfuncs.pfnCmd_Argv (i);

      bool supercede = gs->OnClientCommand (ent, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);

      if (gs.IsMetamod ())
         gs.SetMetaResult (supercede ? MRES_SUPERCEDE : MRES_IGNORED);

      if (supercede)
         return;

      gs.GetDllapiFunctionTable ()->pfnClientCommand (ent);
   };

   table->pfnServerActivate = [] (edict_t *ent, int edictCount, int clientMax)
   {
      gs->OnServerActivated (false);

      // create global killer entity
      gs.CreateKillerEntity ();

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      gs.GetDllapiFunctionTable ()->pfnServerActivate (ent, edictCount, clientMax);

      gs->OnServerActivated (true);
   };

   table->pfnServerDeactivate = [] (void)
   {
      gs->OnServerDeactivated ();

      // terminate killer entity
      gs.DestroyKillerEntity ();

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      gs.GetDllapiFunctionTable ()->pfnServerDeactivate ();
   };

   table->pfnStartFrame = [] (void)
   {
      // pre frame
      gs->OnEngineFrame (false);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      gs.GetDllapiFunctionTable ()->pfnStartFrame ();

      // post frame
      gs->OnEngineFrame (true);
   };

   table->pfnUpdateClientData = [] (const struct edict_t *ent, int sendweapons, struct clientdata_s *cd)
   {
      gs->OnUpdateClientData (const_cast <edict_t *> (ent));

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      gs.GetDllapiFunctionTable ()->pfnUpdateClientData (ent, sendweapons, cd);
   };
   return 1;
}

SHARED_LIBRARAY_EXPORT int GetEntityAPI2_Post (gamefuncs_t *table, int *)
{
   // this function is called right after FuncPointers_t() by the engine in the game DLL (or
   // what it BELIEVES to be the game DLL), in order to copy the list of MOD functions that can
   // be called by the engine, into a memory block pointed to by the functionTable pointer
   // that is passed into this function (explanation comes straight from botman). This allows
   // the Half-Life engine to call these MOD DLL functions when it needs to spawn an entity,
   // connect or disconnect a player, call Think() functions, Touch() functions, or Use()
   // functions, etc. The bot DLL passes its OWN list of these functions back to the Half-Life
   // engine, and then calls the MOD DLL's version of GetEntityAPI to get the REAL gamedll
   // functions this time (to use in the bot code). Post version, called only by metamod.

   memset (table, 0, sizeof (gamefuncs_t));

   table->pfnSpawn = [] (edict_t *ent)
   {
      // solves the bots unable to see through certain types of glass bug.
      if (ent->v.rendermode == kRenderTransTexture)
         ent->v.flags &= ~FL_WORLDBRUSH; // clear the FL_WORLDBRUSH flag out of transparent ents

 
      gs.SetMetaResult (MRES_IGNORED);
      return 0;
   };

   table->pfnStartFrame = [] (void)
   {
      gs->OnEngineFrame (true);
      gs.SetMetaResult (MRES_IGNORED);
   };

   table->pfnServerActivate = [] (edict_t *, int, int)
   {
      gs->OnServerActivated (true);
      gs.SetMetaResult (MRES_IGNORED);
   };
   return 1;
}

SHARED_LIBRARAY_EXPORT int GetNewDLLFunctions (newgamefuncs_t *table, int *interfaceVersion)
{
   // it appears that an extra function table has been added in the engine to gamedll interface
   // since the date where the first enginefuncs table standard was frozen. These ones are
   // facultative and we don't hook them, but since some MODs might be featuring it, we have to
   // pass them too, else the DLL interfacing wouldn't be complete and the game possibly wouldn't
   // run properly.

   return gs.ExecuteNewEntityAPIs (table, *interfaceVersion);
}

SHARED_LIBRARAY_EXPORT int GetEngineFunctions (enginefuncs_t *table, int *)
{
   if (gs.IsMetamod ())
      memset (table, 0, sizeof (enginefuncs_t));

   // called on change level
   table->pfnChangeLevel = [] (char *s1, char *s2)
   {
      gs->OnChangeLevel ();

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnChangeLevel (s1, s2);
   };

   table->pfnFindEntityByString = [] (edict_t *ent, const char *field, const char *value)
   {
      // round starts in counter-strike 1.5
      if ((g_gameFlags & GAME_LEGACY) && strcmp (value, "info_map_parameters") == 0)
         RoundInit ();

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return static_cast <edict_t *> (nullptr);
      }
      return g_engfuncs.pfnFindEntityByString (ent, field, value);
   };

   table->pfnEmitSound = [] (edict_t *entity, int channel, const char *sample, float volume, float attenuation, int flags, int pitch)
   {
      SoundAttachToClients (entity, sample, volume);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnEmitSound (entity, channel, sample, volume, attenuation, flags, pitch);
   };

   table->pfnClientCommand = [] (edict_t *ent, char const *format, ...)
   {
      va_list ap;
      char buffer[MAX_PRINT_BUFFER];

      va_start (ap, format);
      vsnprintf (buffer, SIZEOF_CHAR (buffer), format, ap);
      va_end (ap);

      // is the target entity an official bot, or a third party bot ?
      if (ent->v.flags & FL_FAKECLIENT)
      {
         if (gs.IsMetamod ())
         {
            gs.SetMetaResult (MRES_SUPERCEDE);
            return;
         }
         return;
      }

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnClientCommand (ent, buffer);
   };

   table->pfnMessageBegin = [] (int msgDest, int msgType, const float *origin, edict_t *ed)
   {
      // this function called each time a message is about to sent.

      // store the message type in our own variables, since the GET_USER_MSG_ID () will just do a lot of strcmp()'s...
      if (gs.IsMetamod () && engine.FindMessageId (NETMSG_MONEY) == -1)
      {
         engine.AssignMessageId (NETMSG_VGUI, gs.Meta_GetUserMsgId ("VGUIMenu"));
         engine.AssignMessageId (NETMSG_SHOWMENU, gs.Meta_GetUserMsgId ("ShowMenu"));
         engine.AssignMessageId (NETMSG_WEAPONLIST, gs.Meta_GetUserMsgId ("WeaponList"));
         engine.AssignMessageId (NETMSG_CURWEAPON, gs.Meta_GetUserMsgId ("CurWeapon"));
         engine.AssignMessageId (NETMSG_AMMOX, gs.Meta_GetUserMsgId ("AmmoX"));
         engine.AssignMessageId (NETMSG_AMMOPICKUP, gs.Meta_GetUserMsgId ("AmmoPickup"));
         engine.AssignMessageId (NETMSG_DAMAGE, gs.Meta_GetUserMsgId ("Damage"));
         engine.AssignMessageId (NETMSG_MONEY, gs.Meta_GetUserMsgId ("Money"));
         engine.AssignMessageId (NETMSG_STATUSICON, gs.Meta_GetUserMsgId ("StatusIcon"));
         engine.AssignMessageId (NETMSG_DEATH, gs.Meta_GetUserMsgId ("DeathMsg"));
         engine.AssignMessageId (NETMSG_SCREENFADE, gs.Meta_GetUserMsgId ("ScreenFade"));
         engine.AssignMessageId (NETMSG_HLTV, gs.Meta_GetUserMsgId ("HLTV"));
         engine.AssignMessageId (NETMSG_TEXTMSG, gs.Meta_GetUserMsgId ("TextMsg"));
         engine.AssignMessageId (NETMSG_SCOREINFO, gs.Meta_GetUserMsgId ("ScoreInfo"));
         engine.AssignMessageId (NETMSG_BARTIME, gs.Meta_GetUserMsgId ("BarTime"));
         engine.AssignMessageId (NETMSG_SENDAUDIO, gs.Meta_GetUserMsgId ("SendAudio"));
         engine.AssignMessageId (NETMSG_SAYTEXT, gs.Meta_GetUserMsgId ("SayText"));

         if (!(g_gameFlags & GAME_LEGACY))
            engine.AssignMessageId (NETMSG_BOTVOICE, gs.Meta_GetUserMsgId ("BotVoice"));
      }
      engine.ResetMessageCapture ();

      if ((!(g_gameFlags & GAME_LEGACY) || (g_gameFlags & GAME_XASH)) && msgDest == MSG_SPEC && msgType == engine.FindMessageId (NETMSG_HLTV))
         engine.SetOngoingMessageId (NETMSG_HLTV);

      engine.TryCaptureMessage (msgType, NETMSG_WEAPONLIST);

      if (!engine.IsNullEntity (ed))
      {
         int index = bots.GetIndex (ed);

         // is this message for a bot?
         if (index != -1 && !(ed->v.flags & FL_DORMANT))
         {
            engine.ResetMessageCapture ();
            engine.SetOngoingMessageReceiver (index);

            // message handling is done in usermsg.cpp
            engine.TryCaptureMessage (msgType, NETMSG_VGUI);
            engine.TryCaptureMessage (msgType, NETMSG_CURWEAPON);
            engine.TryCaptureMessage (msgType, NETMSG_AMMOX);
            engine.TryCaptureMessage (msgType, NETMSG_AMMOPICKUP);
            engine.TryCaptureMessage (msgType, NETMSG_DAMAGE);
            engine.TryCaptureMessage (msgType, NETMSG_MONEY);
            engine.TryCaptureMessage (msgType, NETMSG_STATUSICON);
            engine.TryCaptureMessage (msgType, NETMSG_SCREENFADE);
            engine.TryCaptureMessage (msgType, NETMSG_BARTIME);
            engine.TryCaptureMessage (msgType, NETMSG_TEXTMSG);
            engine.TryCaptureMessage (msgType, NETMSG_SHOWMENU);
         }
      }
      else if (msgDest == MSG_ALL)
      {
         engine.ResetMessageCapture ();

         engine.TryCaptureMessage (msgType, NETMSG_SCOREINFO);
         engine.TryCaptureMessage (msgType, NETMSG_DEATH);
         engine.TryCaptureMessage (msgType, NETMSG_TEXTMSG);

         if (msgType == SVC_INTERMISSION)
         {
            for (int i = 0; i < engine.MaxClients (); i++)
            {
               Bot *bot = bots.GetBot (i);

               if (bot != nullptr)
                  bot->m_notKilled = false;
            }
         }
      }

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnMessageBegin (msgDest, msgType, origin, ed);
   };

   table->pfnMessageEnd = [] (void)
   {
      engine.ResetMessageCapture ();

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnMessageEnd ();

      // send latency fix
      bots.SendDeathMsgFix ();
   };

   table->pfnWriteByte = [] (int value)
   {
      // if this message is for a bot, call the client message function...
      engine.ProcessMessageCapture ((void *) &value);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnWriteByte (value);
   };

   table->pfnWriteChar = [] (int value)
   {
      // if this message is for a bot, call the client message function...
      engine.ProcessMessageCapture ((void *) &value);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnWriteChar (value);
   };

   table->pfnWriteShort = [] (int value)
   {
      // if this message is for a bot, call the client message function...
      engine.ProcessMessageCapture ((void *) &value);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnWriteShort (value);
   };

   table->pfnWriteLong = [] (int value)
   {
      // if this message is for a bot, call the client message function...
      engine.ProcessMessageCapture ((void *) &value);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnWriteLong (value);
   };

   table->pfnWriteAngle = [] (float value)
   {
      // if this message is for a bot, call the client message function...
      engine.ProcessMessageCapture ((void *) &value);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnWriteAngle (value);
   };

   table->pfnWriteCoord = [] (float value)
   {
      // if this message is for a bot, call the client message function...
      engine.ProcessMessageCapture ((void *) &value);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnWriteCoord (value);
   };

   table->pfnWriteString = [] (const char *sz)
   {
      // if this message is for a bot, call the client message function...
      engine.ProcessMessageCapture ((void *) sz);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnWriteString (sz);
   };

   table->pfnWriteEntity = [] (int value)
   {
      // if this message is for a bot, call the client message function...
      engine.ProcessMessageCapture ((void *) &value);

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnWriteEntity (value);
   };

   table->pfnRegUserMsg = [] (const char *name, int size)
   {
      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return 0;
      }
      int message = g_engfuncs.pfnRegUserMsg (name, size);

      if (strcmp (name, "VGUIMenu") == 0)
         engine.AssignMessageId (NETMSG_VGUI, message);
      else if (strcmp (name, "ShowMenu") == 0)
         engine.AssignMessageId (NETMSG_SHOWMENU, message);
      else if (strcmp (name, "WeaponList") == 0)
         engine.AssignMessageId (NETMSG_WEAPONLIST, message);
      else if (strcmp (name, "CurWeapon") == 0)
         engine.AssignMessageId (NETMSG_CURWEAPON, message);
      else if (strcmp (name, "AmmoX") == 0)
         engine.AssignMessageId (NETMSG_AMMOX, message);
      else if (strcmp (name, "AmmoPickup") == 0)
         engine.AssignMessageId (NETMSG_AMMOPICKUP, message);
      else if (strcmp (name, "Damage") == 0)
         engine.AssignMessageId (NETMSG_DAMAGE, message);
      else if (strcmp (name, "Money") == 0)
         engine.AssignMessageId (NETMSG_MONEY, message);
      else if (strcmp (name, "StatusIcon") == 0)
         engine.AssignMessageId (NETMSG_STATUSICON, message);
      else if (strcmp (name, "DeathMsg") == 0)
         engine.AssignMessageId (NETMSG_DEATH, message);
      else if (strcmp (name, "ScreenFade") == 0)
         engine.AssignMessageId (NETMSG_SCREENFADE, message);
      else if (strcmp (name, "HLTV") == 0)
         engine.AssignMessageId (NETMSG_HLTV, message);
      else if (strcmp (name, "TextMsg") == 0)
         engine.AssignMessageId (NETMSG_TEXTMSG, message);
      else if (strcmp (name, "ScoreInfo") == 0)
         engine.AssignMessageId (NETMSG_SCOREINFO, message);
      else if (strcmp (name, "BarTime") == 0)
         engine.AssignMessageId (NETMSG_BARTIME, message);
      else if (strcmp (name, "SendAudio") == 0)
         engine.AssignMessageId (NETMSG_SENDAUDIO, message);
      else if (strcmp (name, "SayText") == 0)
         engine.AssignMessageId (NETMSG_SAYTEXT, message);
      else if (strcmp (name, "BotVoice") == 0)
         engine.AssignMessageId (NETMSG_BOTVOICE, message);

      return message;
   };

   table->pfnClientPrintf = [] (edict_t *ent, int printType, const char *message)
   {
      if (IsValidBot (ent))
      {
         if (gs.IsMetamod ())
         {
            gs.SetMetaResult (MRES_SUPERCEDE);
            return;
         }
         return;
      }

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnClientPrintf (ent, printType, message);
   };

   table->pfnCmd_Args = [] (void)
   {
      // is this a bot issuing that client command?
      if (engine.IsBotCommand ())
      {
         if (gs.IsMetamod ())
            gs.SetMetaResult (MRES_SUPERCEDE);

         return engine.GetOverrideArgs (); // else return the whole bot client command string we know
      }
      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return static_cast <const char *> (nullptr);
      }
      return g_engfuncs.pfnCmd_Args (); // ask the client command string to the engine
   };

   table->pfnCmd_Argv = [] (int argc)
   {
      // is this a bot issuing that client command?
      if (engine.IsBotCommand ())
      {
         if (gs.IsMetamod ())
            gs.SetMetaResult (MRES_SUPERCEDE);

         return engine.GetOverrideArgv (argc); // if so, then return the wanted argument we know
      }
      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return static_cast <const char *> (nullptr);
      }
      return g_engfuncs.pfnCmd_Argv (argc); // ask the argument number "argc" to the engine
   };

   table->pfnCmd_Argc = [] (void)
   {
      // is this a bot issuing that client command?
      if (engine.IsBotCommand ())
      {
         if (gs.IsMetamod ())
            gs.SetMetaResult (MRES_SUPERCEDE);

         return engine.GetOverrideArgc (); // if so, then return the argument count we know
      }

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return 0;
      }
      return g_engfuncs.pfnCmd_Argc (); // ask the engine how many arguments there are
   };

   table->pfnSetClientMaxspeed = [] (const edict_t *ent, float newMaxspeed)
   {
      Bot *bot = bots.GetBot (const_cast <edict_t *> (ent));

      // check wether it's not a bot
      if (bot != nullptr)
         bot->pev->maxspeed = newMaxspeed;

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnSetClientMaxspeed (ent, newMaxspeed);
   };

   table->pfnAlertMessage = [] (int alertType, char *format, ...)
   {
      va_list ap;
      char buffer[1024];

      va_start (ap, format);
      vsnprintf (buffer, SIZEOF_CHAR (buffer), format, ap);
      va_end (ap);

      if ((g_mapType & MAP_DE) && g_bombPlanted && strstr (buffer, "_Defuse_") != nullptr)
      {
         // notify all terrorists that CT is starting bomb defusing
         for (int i = 0; i < engine.MaxClients (); i++)
         {
            Bot *bot = bots.GetBot (i);

            if (bot != nullptr && bot->m_team == TERRORIST && bot->m_notKilled)
            {
               bot->ResetTasks ();
               bot->MoveToVector (waypoints.GetBombPosition ());
            }
         }
      }

      if (gs.IsMetamod ())
      {
         gs.SetMetaResult (MRES_IGNORED);
         return;
      }
      g_engfuncs.pfnAlertMessage (alertType, buffer);
   };
   return 1;
}

SHARED_LIBRARAY_EXPORT int GetEngineFunctions_Post (enginefuncs_t *table, int *)
{
   memset (table, 0, sizeof (enginefuncs_t));

   table->pfnMessageEnd = [] (void)
   {
      // send latency fix
      bots.SendDeathMsgFix ();

      if (gs.IsMetamod ())
         gs.SetMetaResult (MRES_IGNORED);
   };
   return 1;
}

SHARED_LIBRARAY_EXPORT int Server_GetBlendingInterface (int version, void **ppinterface, void *pstudio, float (*rotationmatrix)[3][4], float (*bonetransform)[128][3][4])
{
   // this function synchronizes the studio model animation blending interface (i.e, what parts
   // of the body move, which bones, which hitboxes and how) between the server and the game DLL.
   // some MODs can be using a different hitbox scheme than the standard one.

   auto addr = g_gameLib->GetFuncAddr <GETBLENDINGINTERFACE_FN> ("Server_GetBlendingInterface");

   if (addr == nullptr)
   {
      AddLogEntry (false, LL_WARNING, "Failed to find Server_GetBlendingInterface() export in game dll. Probably old gamedll?");
      return 0;
   }

   if (!addr (version, ppinterface, pstudio, rotationmatrix, bonetransform))
   {
      AddLogEntry (true, LL_FATAL, "Gamedll version of Server_GetBlendingInterface() returned zero. Please verify that file is valid.");
      return FALSE;
   }
   return addr (version, ppinterface, pstudio, rotationmatrix, bonetransform);
}

SHARED_LIBRARAY_EXPORT int Meta_Query (char *, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs)
{
   // this function is the first function ever called by metamod in the plugin DLL. Its purpose
   // is for metamod to retrieve basic information about the plugin, such as its meta-interface
   // version, for ensuring compatibility with the current version of the running metamod.

   gs.ExecuteMetaQuery (pPlugInfo, pMetaUtilFuncs);

   return 1; // tell metamod this plugin looks safe
}

SHARED_LIBRARAY_EXPORT int Meta_Attach (int, metamod_funcs_t *table, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs)
{
   // this function is called when metamod attempts to load the plugin. Since it's the place
   // where we can tell if the plugin will be allowed to run or not, we wait until here to make
   // our initialization stuff, like registering CVARs and dedicated server commands.

   gs.ExecuteMetaAttach (table, pMGlobals, pGamedllFuncs);

   return 1; // returning true enables metamod to attach this plugin
}

SHARED_LIBRARAY_EXPORT int Meta_Detach (int, int)
{
   // this function is called when metamod unloads the plugin. A basic check is made in order
   // to prevent unloading the plugin if its processing should not be interrupted.

   bots.RemoveAll (); // kick all bots off this server
   FreeBotMemory ();

   return 1;
}

SHARED_LIBRARAY_EXPORT void Meta_Init (void)
{
   // this function is called by metamod, before any other interface functions. Purpose of this
   // function to give plugin a chance to determine is plugin running under metamod or not.

   gs.SetupMetamod ();
}

DLL_GIVEFNPTRSTODLL GiveFnptrsToDll (enginefuncs_t *table, globalvars_t *globals)
{
   // this is the very first function that is called in the game DLL by the engine. Its purpose
   // is to set the functions interfacing up, by exchanging the functionTable function list
   // along with a pointer to the engine's global variables structure pGlobals, with the game
   // DLL. We can there decide if we want to load the normal game DLL just behind our bot DLL,
   // or any other game DLL that is present, such as Will Day's metamod. Also, since there is
   // a known bug on Win32 platforms that prevent hook DLLs (such as our bot DLL) to be used in
   // single player games (because they don't export all the stuff they should), we may need to
   // build our own array of exported symbols from the actual game DLL in order to use it as
   // such if necessary. Nothing really bot-related is done in this function. The actual bot
   // initialization stuff will be done later, when we'll be certain to have a multilayer game.

   // get the engine functions from the engine...
   memcpy (&g_engfuncs, table, sizeof (enginefuncs_t));
   g_pGlobals = globals;

   // register our cvars
   engine.PushRegisteredConVarsToEngine ();

   static GameEventsListener ge;
   gs.SetEventListener (&ge);

   // ensure we're have all needed directories
   {
      const char *mod = engine.GetModName ();

      // create the needed paths
      File::CreatePath (const_cast <char *> (FormatBuffer ("%s/addons/yapb/conf/lang", mod)));
      File::CreatePath (const_cast <char *> (FormatBuffer ("%s/addons/yapb/data/learned", mod)));
   }

#ifdef PLATFORM_ANDROID
   g_gameFlags |= (GAME_LEGACY | GAME_XASH | GAME_MOBILITY);

   if (gs.IsMetamod ())
      return;  // we should stop the attempt for loading the real gamedll, since metamod handle this for us

#ifdef LOAD_HARDFP
   const char *serverDLL = "libserver_hardfp.so";
#else
   const char *serverDLL = "libserver.so";
#endif

   char gameDLLName[256];
   snprintf (gameDLLName, SIZEOF_CHAR (gameDLLName), "%s/%s", getenv ("XASH3D_GAMELIBDIR"), serverDLL);

   g_gameLib = new Library (gameDLLName);

   if (!g_gameLib->IsLoaded ())
      AddLogEntry (true, LL_FATAL | LL_IGNORE, "Unable to load gamedll \"%s\". Exiting... (gamedir: %s)", gameDLLName, GetModName ());
#else
   static struct ModSupport
   {
      char name[10];
      char linuxLib[12];
      char osxLib[9];
      char winLib[8];
      char desc[39];
      int modType;
   } s_supportedMods[] =
   {
      { "cstrike", "cs_i386.so", "cs.dylib", "mp.dll", "Counter-Strike v1.6", GAME_CSTRIKE16 },
      { "cstrike", "cs.so", "cs.dylib", "mp.dll", "Counter-Strike v1.6 (Newer)", GAME_CSTRIKE16 },
      { "czero", "cs_i386.so", "cs.dylib", "mp.dll", "Counter-Strike: Condition Zero", GAME_CZERO },
      { "czero", "cs.so", "cs.dylib", "mp.dll", "Counter-Strike: Condition Zero (Newer)", GAME_CZERO },
      { "csv15", "cs_i386.so", "cs.dylib", "mp.dll", "CS 1.5 for Steam", GAME_LEGACY },
      { "csdm", "cs_i386.so", "cs.dylib", "mp.dll", "CSDM for Windows", GAME_LEGACY },
      { "cs13", "cs_i386.so", "cs.dylib", "mp.dll", "Counter-Strike v1.3", GAME_LEGACY }, // assume cs13 = cs15
   };

   ModSupport *knownMod = nullptr;

   for (int i = 0; i < SIZEOF_ARRAY (s_supportedMods); i++)
   {
      ModSupport *mod = &s_supportedMods[i];

      if (strcmp (mod->name, engine.GetModName ()) == 0 && File::Accessible (FormatBuffer ("%s/dlls/%s", mod->name,
#if defined (PLATFORM_WIN32)
         mod->winLib
#elif defined (PLATFORM_LINUX)
         mod->linuxLib
#elif defined (PLATFORM_OSX)
         mod->osxLib
#endif
         )))
      {
         knownMod = mod;
         break;
      }
   }

   if (knownMod != nullptr)
   {
      g_gameFlags |= knownMod->modType;

      if (gs.IsMetamod ())
         return; // we should stop the attempt for loading the real gamedll, since metamod handle this for us

      char gameDLLName[256];
      sprintf (gameDLLName, "%s/dlls/%s", knownMod->name,

#if defined (PLATFORM_WIN32)
         knownMod->winLib
#elif defined (PLATFORM_LINUX)
         knownMod->linuxLib
#elif defined (PLATFORM_OSX)
         knownMod->osxLib
#endif
      );
      g_gameLib = new Library (gameDLLName);

      if (!g_gameLib->IsLoaded ())
         AddLogEntry (true, LL_FATAL | LL_IGNORE, "Unable to load gamedll \"%s\". Exiting... (gamedir: %s)", gameDLLName, engine.GetModName ());
   }
   else
      AddLogEntry (true, LL_FATAL | LL_IGNORE, "Mod that you has started, not supported by this bot (gamedir: %s)", engine.GetModName ());
#endif

   auto addr = g_gameLib->GetFuncAddr <GIVEFNPTRSTODLL_FN> ("GiveFnptrsToDll");

   // if we have no this addr, it's fatal error
   if (addr == nullptr)
   {
      AddLogEntry (false, LL_FATAL, "Failed to find GiveFnptrsToDll() export in game dll. Please verify that file is valid.");
      return;
   }
   GetEngineFunctions (table, nullptr);

   // give the engine functions to the other DLL...
   addr (table, globals);
}

DLL_ENTRYPOINT
{
   // dynamic library entry point, can be used for uninitialization stuff. NOT for initializing
   // anything because if you ever attempt to wander outside the scope of this function on a
   // DLL attach, LoadLibrary() will simply fail. And you can't do I/Os here either.

   // dynamic library detaching ??
   if (DLL_DETACHING)
   {
      FreeBotMemory (); // free everything that's free able
      delete g_gameLib; // if dynamic link library of mod is load, free it
   }
   DLL_RETENTRY; // the return data type is OS specific too
}

void LinkEntity_Helper (PASSGAMEENTITY_FN &addr, const char *name, entvars_t *pev)
{
   if (addr == nullptr)
      addr = g_gameLib->GetFuncAddr <PASSGAMEENTITY_FN > (name);

   if (addr == nullptr)
      return;

   addr (pev);
}

#define LINK_ENTITY(entityName) \
SHARED_LIBRARAY_EXPORT void entityName (entvars_t *pev) \
{ \
   static PASSGAMEENTITY_FN addr = nullptr; \
   LinkEntity_Helper (addr, #entityName, pev); \
} \

// entities in counter-strike...
LINK_ENTITY (DelayedUse)
LINK_ENTITY (ambient_generic)
LINK_ENTITY (ammo_338magnum)
LINK_ENTITY (ammo_357sig)
LINK_ENTITY (ammo_45acp)
LINK_ENTITY (ammo_50ae)
LINK_ENTITY (ammo_556nato)
LINK_ENTITY (ammo_556natobox)
LINK_ENTITY (ammo_57mm)
LINK_ENTITY (ammo_762nato)
LINK_ENTITY (ammo_9mm)
LINK_ENTITY (ammo_buckshot)
LINK_ENTITY (armoury_entity)
LINK_ENTITY (beam)
LINK_ENTITY (bodyque)
LINK_ENTITY (button_target)
LINK_ENTITY (cycler)
LINK_ENTITY (cycler_prdroid)
LINK_ENTITY (cycler_sprite)
LINK_ENTITY (cycler_weapon)
LINK_ENTITY (cycler_wreckage)
LINK_ENTITY (env_beam)
LINK_ENTITY (env_beverage)
LINK_ENTITY (env_blood)
LINK_ENTITY (env_bombglow)
LINK_ENTITY (env_bubbles)
LINK_ENTITY (env_debris)
LINK_ENTITY (env_explosion)
LINK_ENTITY (env_fade)
LINK_ENTITY (env_funnel)
LINK_ENTITY (env_global)
LINK_ENTITY (env_glow)
LINK_ENTITY (env_laser)
LINK_ENTITY (env_lightning)
LINK_ENTITY (env_message)
LINK_ENTITY (env_rain)
LINK_ENTITY (env_render)
LINK_ENTITY (env_shake)
LINK_ENTITY (env_shooter)
LINK_ENTITY (env_snow)
LINK_ENTITY (env_sound)
LINK_ENTITY (env_spark)
LINK_ENTITY (env_sprite)
LINK_ENTITY (fireanddie)
LINK_ENTITY (func_bomb_target)
LINK_ENTITY (func_breakable)
LINK_ENTITY (func_button)
LINK_ENTITY (func_buyzone)
LINK_ENTITY (func_conveyor)
LINK_ENTITY (func_door)
LINK_ENTITY (func_door_rotating)
LINK_ENTITY (func_escapezone)
LINK_ENTITY (func_friction)
LINK_ENTITY (func_grencatch)
LINK_ENTITY (func_guntarget)
LINK_ENTITY (func_healthcharger)
LINK_ENTITY (func_hostage_rescue)
LINK_ENTITY (func_illusionary)
LINK_ENTITY (func_ladder)
LINK_ENTITY (func_monsterclip)
LINK_ENTITY (func_mortar_field)
LINK_ENTITY (func_pendulum)
LINK_ENTITY (func_plat)
LINK_ENTITY (func_platrot)
LINK_ENTITY (func_pushable)
LINK_ENTITY (func_rain)
LINK_ENTITY (func_recharge)
LINK_ENTITY (func_rot_button)
LINK_ENTITY (func_rotating)
LINK_ENTITY (func_snow)
LINK_ENTITY (func_tank)
LINK_ENTITY (func_tankcontrols)
LINK_ENTITY (func_tanklaser)
LINK_ENTITY (func_tankmortar)
LINK_ENTITY (func_tankrocket)
LINK_ENTITY (func_trackautochange)
LINK_ENTITY (func_trackchange)
LINK_ENTITY (func_tracktrain)
LINK_ENTITY (func_train)
LINK_ENTITY (func_traincontrols)
LINK_ENTITY (func_vehicle)
LINK_ENTITY (func_vehiclecontrols)
LINK_ENTITY (func_vip_safetyzone)
LINK_ENTITY (func_wall)
LINK_ENTITY (func_wall_toggle)
LINK_ENTITY (func_water)
LINK_ENTITY (func_weaponcheck)
LINK_ENTITY (game_counter)
LINK_ENTITY (game_counter_set)
LINK_ENTITY (game_end)
LINK_ENTITY (game_player_equip)
LINK_ENTITY (game_player_hurt)
LINK_ENTITY (game_player_team)
LINK_ENTITY (game_score)
LINK_ENTITY (game_team_master)
LINK_ENTITY (game_team_set)
LINK_ENTITY (game_text)
LINK_ENTITY (game_zone_player)
LINK_ENTITY (gibshooter)
LINK_ENTITY (grenade)
LINK_ENTITY (hostage_entity)
LINK_ENTITY (info_bomb_target)
LINK_ENTITY (info_hostage_rescue)
LINK_ENTITY (info_intermission)
LINK_ENTITY (info_landmark)
LINK_ENTITY (info_map_parameters)
LINK_ENTITY (info_null)
LINK_ENTITY (info_player_deathmatch)
LINK_ENTITY (info_player_start)
LINK_ENTITY (info_target)
LINK_ENTITY (info_teleport_destination)
LINK_ENTITY (info_vip_start)
LINK_ENTITY (infodecal)
LINK_ENTITY (item_airtank)
LINK_ENTITY (item_antidote)
LINK_ENTITY (item_assaultsuit)
LINK_ENTITY (item_battery)
LINK_ENTITY (item_healthkit)
LINK_ENTITY (item_kevlar)
LINK_ENTITY (item_longjump)
LINK_ENTITY (item_security)
LINK_ENTITY (item_sodacan)
LINK_ENTITY (item_suit)
LINK_ENTITY (item_thighpack)
LINK_ENTITY (light)
LINK_ENTITY (light_environment)
LINK_ENTITY (light_spot)
LINK_ENTITY (momentary_door)
LINK_ENTITY (momentary_rot_button)
LINK_ENTITY (monster_hevsuit_dead)
LINK_ENTITY (monster_mortar)
LINK_ENTITY (monster_scientist)
LINK_ENTITY (multi_manager)
LINK_ENTITY (multisource)
LINK_ENTITY (path_corner)
LINK_ENTITY (path_track)
LINK_ENTITY (player)
LINK_ENTITY (player_loadsaved)
LINK_ENTITY (player_weaponstrip)
LINK_ENTITY (soundent)
LINK_ENTITY (spark_shower)
LINK_ENTITY (speaker)
LINK_ENTITY (target_cdaudio)
LINK_ENTITY (test_effect)
LINK_ENTITY (trigger)
LINK_ENTITY (trigger_auto)
LINK_ENTITY (trigger_autosave)
LINK_ENTITY (trigger_camera)
LINK_ENTITY (trigger_cdaudio)
LINK_ENTITY (trigger_changelevel)
LINK_ENTITY (trigger_changetarget)
LINK_ENTITY (trigger_counter)
LINK_ENTITY (trigger_endsection)
LINK_ENTITY (trigger_gravity)
LINK_ENTITY (trigger_hurt)
LINK_ENTITY (trigger_monsterjump)
LINK_ENTITY (trigger_multiple)
LINK_ENTITY (trigger_once)
LINK_ENTITY (trigger_push)
LINK_ENTITY (trigger_relay)
LINK_ENTITY (trigger_teleport)
LINK_ENTITY (trigger_transition)
LINK_ENTITY (weapon_ak47)
LINK_ENTITY (weapon_aug)
LINK_ENTITY (weapon_awp)
LINK_ENTITY (weapon_c4)
LINK_ENTITY (weapon_deagle)
LINK_ENTITY (weapon_elite)
LINK_ENTITY (weapon_famas)
LINK_ENTITY (weapon_fiveseven)
LINK_ENTITY (weapon_flashbang)
LINK_ENTITY (weapon_g3sg1)
LINK_ENTITY (weapon_galil)
LINK_ENTITY (weapon_glock18)
LINK_ENTITY (weapon_hegrenade)
LINK_ENTITY (weapon_knife)
LINK_ENTITY (weapon_m249)
LINK_ENTITY (weapon_m3)
LINK_ENTITY (weapon_m4a1)
LINK_ENTITY (weapon_mac10)
LINK_ENTITY (weapon_mp5navy)
LINK_ENTITY (weapon_p228)
LINK_ENTITY (weapon_p90)
LINK_ENTITY (weapon_scout)
LINK_ENTITY (weapon_sg550)
LINK_ENTITY (weapon_sg552)
LINK_ENTITY (weapon_shield)
LINK_ENTITY (weapon_shieldgun)
LINK_ENTITY (weapon_smokegrenade)
LINK_ENTITY (weapon_tmp)
LINK_ENTITY (weapon_ump45)
LINK_ENTITY (weapon_usp)
LINK_ENTITY (weapon_xm1014)
LINK_ENTITY (weaponbox)
LINK_ENTITY (world_items)
LINK_ENTITY (worldspawn)

#ifdef XASH_CSDM
LINK_ENTITY (aiscripted_sequence)
LINK_ENTITY (cine_blood)
LINK_ENTITY (deadplayer_entity)
LINK_ENTITY (func_headq)
LINK_ENTITY (info_node)
LINK_ENTITY (info_node_air)
LINK_ENTITY (info_player_csdm)
LINK_ENTITY (monster_c4)
LINK_ENTITY (monster_cine2_hvyweapons)
LINK_ENTITY (monster_cine2_scientist)
LINK_ENTITY (monster_cine2_slave)
LINK_ENTITY (monster_cine3_barney)
LINK_ENTITY (monster_cine3_scientist)
LINK_ENTITY (monster_cine_barney)
LINK_ENTITY (monster_cine_panther)
LINK_ENTITY (monster_cine_scientist)
LINK_ENTITY (monster_cockroach)
LINK_ENTITY (monster_furniture)
LINK_ENTITY (monster_osprey)
LINK_ENTITY (monster_rat)
LINK_ENTITY (monster_tentacle)
LINK_ENTITY (monster_tentaclemaw)
LINK_ENTITY (monstermaker)
LINK_ENTITY (node_viewer)
LINK_ENTITY (node_viewer_fly)
LINK_ENTITY (node_viewer_human)
LINK_ENTITY (node_viewer_large)
LINK_ENTITY (scripted_sequence)
LINK_ENTITY (testhull)
LINK_ENTITY (xen_hair)
LINK_ENTITY (xen_hull)
LINK_ENTITY (xen_plantlight)
LINK_ENTITY (xen_spore_large)
LINK_ENTITY (xen_spore_medium)
LINK_ENTITY (xen_spore_small)
LINK_ENTITY (xen_tree)
LINK_ENTITY (xen_ttrigger)
#endif

int GoldSrcLoader::Meta_GetUserMsgId (const char *msgname)
{
   return m_metaUtilityFunctions->pfnGetUserMsgID (&m_pluginInfo, msgname, nullptr);
}

int GoldSrcLoader::CallPlayerEntity (entvars_t *pev)
{
   if (m_isMetamod)
      return m_metaUtilityFunctions->pfnCallGameEntity (&m_pluginInfo, "player", pev);

   player (pev);
   return 0;
}

void GoldSrcLoader::ExecuteEntityAPIs (gamefuncs_t *table)
{
   memset (table, 0, sizeof (gamefuncs_t));

   if (m_isMetamod)
      return;

   auto addr = g_gameLib->GetFuncAddr <GETENTITYAPI_FN> ("GetEntityAPI");

   if (addr == nullptr)
   {
      AddLogEntry (false, LL_FATAL, "Failed to find GetEntityAPI() export in game dll. Please verify that file is valid.");
      return;
   }

   if (addr (&m_dllapiFunctions, GAME_INTERFACE_VERSION) == 0)
   {
      AddLogEntry (true, LL_FATAL, "Gamedll version of GetEntityAPI() returned zero. Please verify that file is valid.");
      return;
   }

   m_metaFakeFunctions.dllapi_table = &m_dllapiFunctions;
   m_metaGameFunctions = &m_metaFakeFunctions;

   memcpy (table, &m_dllapiFunctions, sizeof (gamefuncs_t));
}

int GoldSrcLoader::ExecuteNewEntityAPIs (newgamefuncs_t *table, int version)
{
   if (m_isMetamod)
      return 1;

   auto addr = g_gameLib->GetFuncAddr <GETNEWDLLFUNCTIONS_FN> ("GetNewDLLFunctions");

   if (addr == nullptr)
   {
      AddLogEntry (false, LL_WARNING, "Failed to find GetNewDLLFunctions() export in game dll. Please verify that file is valid.");
      return 0;
   }

   if (!addr (table, &version))
   {
      AddLogEntry (true, LL_FATAL, "Gamedll version of GetNewDLLFunctions() returned zero. Please verify that file is valid.");
      return FALSE;
   }
   m_metaFakeFunctions.newapi_table = table;

   return 1;
}

void GoldSrcLoader::ExecuteMetaQuery (plugin_info_t **pluginInfo, mutil_funcs_t *metaUtilityFunctions)
{
   m_metaUtilityFunctions = metaUtilityFunctions;
   *pluginInfo = &m_pluginInfo;
}

void GoldSrcLoader::ExecuteMetaAttach (metamod_funcs_t *table, meta_globals_t *globals, gamedll_funcs_t *gameFunctions)
{
   // keep track of the pointers to engine function tables metamod gives us
   m_metaGlobals = globals;
   m_metaGameFunctions = gameFunctions;

   memcpy (table, &m_metaFunctionTable, sizeof (metamod_funcs_t));
}

void GoldSrcLoader::CreateKillerEntity (void)
{
   // this function creates single trigger_hurt for using in Bot::Kill, to reduce lags, when killing all the bots

   m_killerEntity = g_engfuncs.pfnCreateNamedEntity (ValveString::Make ("trigger_hurt"));

   m_killerEntity->v.dmg = 9999.0f;
   m_killerEntity->v.dmg_take = 1.0f;
   m_killerEntity->v.dmgtime = 2.0f;
   m_killerEntity->v.effects |= EF_NODRAW;

   g_engfuncs.pfnSetOrigin (m_killerEntity, Vector (-99999.0f, -99999.0f, -99999.0f));
   m_metaGameFunctions->dllapi_table->pfnSpawn (m_killerEntity);
}

void GoldSrcLoader::DestroyKillerEntity (void)
{
   if (!engine.IsNullEntity (m_killerEntity))
      g_engfuncs.pfnRemoveEntity (m_killerEntity);
}

void GoldSrcLoader::SlayPlayer (edict_t *ent, const char *weapon)
{
   if (engine.IsNullEntity (m_killerEntity))
   {
      m_metaGameFunctions->dllapi_table->pfnClientKill (ent);
      return;
   }

   m_killerEntity->v.classname = ValveString::Make (weapon);
   m_killerEntity->v.dmg_inflictor = ent;

   KeyValueData kv;
   kv.szClassName = const_cast <char *> (weapon);
   kv.szKeyName = "damagetype";
   kv.szValue = const_cast <char *> (FormatBuffer ("%d", (1 << 4)));
   kv.fHandled = FALSE;

   m_metaGameFunctions->dllapi_table->pfnKeyValue (m_killerEntity, &kv);
   m_metaGameFunctions->dllapi_table->pfnTouch (m_killerEntity, ent);
}

void GoldSrcLoader::SendClientCommands (edict_t *ent)
{
   m_metaGameFunctions->dllapi_table->pfnClientCommand (ent);
}

void GoldSrcLoader::SendUse (edict_t *what, edict_t *who)
{
   m_metaGameFunctions->dllapi_table->pfnUse (what, who);
}

edict_t *GoldSrcLoader::ConstructFakeClient (const char *botName, int pingType, bool avatars, const char *steamID)
{
   edict_t *bot = g_engfuncs.pfnCreateFakeClient (botName);

   if (engine.IsNullEntity (bot))
      return NULL;

   int index = engine.IndexOfEntity (bot);

   if (bot->pvPrivateData != nullptr)
      g_engfuncs.pfnFreeEntPrivateData (bot);

   bot->pvPrivateData = nullptr;
   bot->v.frags = 0;

   // call gamedll player()
   CallPlayerEntity (&bot->v);

   char rejectReason[128];

   char *buffer = g_engfuncs.pfnGetInfoKeyBuffer (bot);
   g_engfuncs.pfnSetClientKeyValue (index, buffer, "_vgui_menus", "0");

   if (!(g_gameFlags & GAME_LEGACY) && pingType == 1)
      g_engfuncs.pfnSetClientKeyValue (index, buffer, "*bot", "1");

   rejectReason[0] = 0; // reset the reject reason template string
   m_metaGameFunctions->dllapi_table->pfnClientConnect (bot, "BOT", FormatBuffer ("127.0.0.%d", index + 100), rejectReason);

   if (avatars && !IsNullString (steamID))
      g_engfuncs.pfnSetClientKeyValue (index, buffer, "*sid", const_cast <char *> (steamID));

   if (!IsNullString (rejectReason))
   {
      AddLogEntry (true, LL_WARNING, "Server refused '%s' connection (%s)", ValveString::Get (bot->v.netname), rejectReason);
      engine.IssueCmd ("kick \"%s\"", ValveString::Get (bot->v.netname));

      bot->v.flags |= FL_KILLME;
   }

   m_metaGameFunctions->dllapi_table->pfnClientPutInServer (bot);
   bot->v.flags |= FL_FAKECLIENT; // set this player as fakeclient

   return bot;
}

void GoldSrcLoader::SetupMetamod (void)
{
   m_isMetamod = true;

   memset (&m_pluginInfo, 0, sizeof (plugin_info_t));
   memset (&m_metaFunctionTable, 0, sizeof (metamod_funcs_t));

   m_metaFunctionTable.pfnGetEngineFunctions = GetEngineFunctions;
   m_metaFunctionTable.pfnGetEngineFunctions_Post = GetEngineFunctions_Post;
   m_metaFunctionTable.pfnGetEntityAPI2 = GetEntityAPI2;
   m_metaFunctionTable.pfnGetEntityAPI2_Post = GetEntityAPI2_Post;
   m_metaFunctionTable.pfnGetEntityAPI = nullptr;
   m_metaFunctionTable.pfnGetEntityAPI_Post = nullptr;
   m_metaFunctionTable.pfnGetNewDLLFunctions = nullptr;
   m_metaFunctionTable.pfnGetNewDLLFunctions_Post = nullptr;

   m_pluginInfo.ifvers = META_INTERFACE_VERSION;
   m_pluginInfo.loadable = PT_CHANGELEVEL;
   m_pluginInfo.unloadable = PT_ANYTIME;
   m_pluginInfo.name = PRODUCT_NAME;
   m_pluginInfo.version = PRODUCT_VERSION;
   m_pluginInfo.date = PRODUCT_DATE;
   m_pluginInfo.author = PRODUCT_AUTHOR;
   m_pluginInfo.url = PRODUCT_URL;
   m_pluginInfo.logtag = PRODUCT_LOGTAG;
}