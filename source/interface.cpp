//
// Yet Another POD-Bot, based on PODBot by Markus Klinge ("CountFloyd").
// Copyright (c) YaPB Development Team.
//
// This software is licensed under the BSD-style license.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     http://yapb.jeefo.net/license
//

#include <core.h>

// console vars
ConVar yb_password ("yb_password", "", VT_PASSWORD);
ConVar yb_password_key ("yb_password_key", "_ybpw");

ConVar yb_language ("yb_language", "en");
ConVar yb_version ("yb_version", PRODUCT_VERSION, VT_READONLY);

ConVar mp_startmoney ("mp_startmoney", nullptr, VT_NOREGISTER);

int BotCommandHandler (edict_t *ent, const char *arg0, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *self)
{
   // adding one bot with random parameters to random team
   if (_stricmp (arg0, "addbot") == 0 || _stricmp (arg0, "add") == 0)
      bots.AddBot (arg4, arg1, arg2, arg3, arg5);

   // adding one bot with high difficulty parameters to random team
   else if (_stricmp (arg0, "addbot_hs") == 0 || _stricmp (arg0, "addhs") == 0)
      bots.AddBot (arg4, "4", "1", arg3, arg5);

   // adding one bot with random parameters to terrorist team
   else if (_stricmp (arg0, "addbot_t") == 0 || _stricmp (arg0, "add_t") == 0)
      bots.AddBot (arg4, arg1, arg2, "1", arg5);

   // adding one bot with random parameters to counter-terrorist team
   else if (_stricmp (arg0, "addbot_ct") == 0 || _stricmp (arg0, "add_ct") == 0)
      bots.AddBot (arg4, arg1, arg2, "2", arg5);
         
   // kicking off one bot from the terrorist team
   else if (_stricmp (arg0, "kickbot_t") == 0 || _stricmp (arg0, "kick_t") == 0)
      bots.RemoveFromTeam (TERRORIST);

   // kicking off one bot from the counter-terrorist team
   else if (_stricmp (arg0, "kickbot_ct") == 0 || _stricmp (arg0, "kick_ct") == 0)
      bots.RemoveFromTeam (CT);

   // kills all bots on the terrorist team
   else if (_stricmp (arg0, "killbots_t") == 0 || _stricmp (arg0, "kill_t") == 0)
      bots.KillAll (TERRORIST);

   // kills all bots on the counter-terrorist team
   else if (_stricmp (arg0, "killbots_ct") == 0 || _stricmp (arg0, "kill_ct") == 0)
      bots.KillAll (CT);

   // list all bots playeing on the server
   else if (_stricmp (arg0, "listbots") == 0 || _stricmp (arg0, "list") == 0)
      bots.ListBots ();

   // kick off all bots from the played server
   else if (_stricmp (arg0, "kickbots") == 0 || _stricmp (arg0, "kickall") == 0)
      bots.RemoveAll ();

   // kill all bots on the played server
   else if (_stricmp (arg0, "killbots") == 0 || _stricmp (arg0, "killall") == 0)
      bots.KillAll ();

   // kick off one random bot from the played server
   else if (_stricmp (arg0, "kickone") == 0 || _stricmp (arg0, "kick") == 0)
      bots.RemoveRandom ();

   // fill played server with bots
   else if (_stricmp (arg0, "fillserver") == 0 || _stricmp (arg0, "fill") == 0)
      bots.FillServer (atoi (arg1), IsNullString (arg2) ? -1 : atoi (arg2), IsNullString (arg3) ? -1 : atoi (arg3), IsNullString (arg4) ? -1 : atoi (arg4));

   // select the weapon mode for bots
   else if (_stricmp (arg0, "weaponmode") == 0 || _stricmp (arg0, "wmode") == 0)
   {
      int selection = atoi (arg1);

      // check is selected range valid
      if (selection >= 1 && selection <= 7)
         bots.SetWeaponMode (selection);
      else
         engine.ClientPrintf (ent, "Choose weapon from 1 to 7 range");
   }

   // force all bots to vote to specified map
   else if (_stricmp (arg0, "votemap") == 0)
   {
      if (!IsNullString (arg1))
      {
         int nominatedMap = atoi (arg1);

         // loop through all players
         for (int i = 0; i < engine.MaxClients (); i++)
         {
            Bot *bot = bots.GetBot (i);

            if (bot != nullptr)
               bot->m_voteMap = nominatedMap;
         }
         engine.ClientPrintf (ent, "All dead bots will vote for map #%d", nominatedMap);
      }
   }

   // displays version information
   else if (_stricmp (arg0, "version") == 0 || _stricmp (arg0, "ver") == 0)
   {
      char versionData[] =
         "------------------------------------------------\n"
         "Name: %s\n"
         "Version: %s (Build: %u)\n"
         "Compiled: %s, %s\n"
         "Git Hash: %s\n"
         "Git Commit Author: %s\n"
         "------------------------------------------------";

      engine.ClientPrintf (ent, versionData, PRODUCT_NAME, PRODUCT_VERSION, GenerateBuildNumber (), __DATE__, __TIME__, PRODUCT_GIT_HASH, PRODUCT_GIT_COMMIT_AUTHOR);
   }

   // display some sort of help information
   else if (_stricmp (arg0, "?") == 0 || _stricmp (arg0, "help") == 0)
   {
      engine.ClientPrintf (ent, "Bot Commands:");
      engine.ClientPrintf (ent, "%s version\t - display version information.", self);
      engine.ClientPrintf (ent, "%s add\t - create a bot in current game.", self);
      engine.ClientPrintf (ent, "%s fill\t - fill the server with random bots.", self);
      engine.ClientPrintf (ent, "%s kickall\t - disconnects all bots from current game.", self);
      engine.ClientPrintf (ent, "%s killbots\t - kills all bots in current game.", self);
      engine.ClientPrintf (ent, "%s kick\t - disconnect one random bot from game.", self);
      engine.ClientPrintf (ent, "%s weaponmode\t - select bot weapon mode.", self);
      engine.ClientPrintf (ent, "%s votemap\t - allows dead bots to vote for specific map.", self);
      engine.ClientPrintf (ent, "%s cmenu\t - displaying bots command menu.", self);

      if (_stricmp (arg1, "full") == 0 || _stricmp (arg1, "f") == 0 || _stricmp (arg1, "?") == 0)
      {
         engine.ClientPrintf (ent, "%s add_t\t - creates one random bot to terrorist team.", self);
         engine.ClientPrintf (ent, "%s add_ct\t - creates one random bot to ct team.", self);
         engine.ClientPrintf (ent, "%s kick_t\t - disconnect one random bot from terrorist team.", self);
         engine.ClientPrintf (ent, "%s kick_ct\t - disconnect one random bot from ct team.", self);
         engine.ClientPrintf (ent, "%s kill_t\t - kills all bots on terrorist team.", self);
         engine.ClientPrintf (ent, "%s kill_ct\t - kills all bots on ct team.", self);
         engine.ClientPrintf (ent, "%s list\t - display list of bots currently playing.", self);
         engine.ClientPrintf (ent, "%s order\t - execute specific command on specified bot.", self);
         engine.ClientPrintf (ent, "%s time\t - displays current time on server.", self);
         engine.ClientPrintf (ent, "%s deletewp\t - erase waypoint file from hard disk (permanently).", self);

          if (!engine.IsDedicatedServer ())
          {
             engine.Printf ("%s autowp\t - toggle autowaypointing.", self);
             engine.Printf ("%s wp\t - toggle waypoint showing.", self);
             engine.Printf ("%s wp on noclip\t - enable noclip cheat", self);
             engine.Printf ("%s wp save nocheck\t - save waypoints without checking.", self);
             engine.Printf ("%s wp add\t - open menu for waypoint creation.", self);
             engine.Printf ("%s wp menu\t - open main waypoint menu.", self);
             engine.Printf ("%s wp addbasic\t - creates basic waypoints on map.", self);
             engine.Printf ("%s wp find\t - show direction to specified waypoint.", self);
             engine.Printf ("%s wp load\t - load the waypoint file from hard disk.", self);
             engine.Printf ("%s wp check\t - checks if all waypoints connections are valid.", self);
             engine.Printf ("%s wp cache\t - cache nearest waypoint.", self);
             engine.Printf ("%s wp teleport\t - teleport hostile to specified waypoint.", self);
             engine.Printf ("%s wp setradius\t - manually sets the wayzone radius for this waypoint.", self);
             engine.Printf ("%s path autodistance - opens menu for setting autopath maximum distance.", self);
             engine.Printf ("%s path cache\t - remember the nearest to player waypoint.", self);
             engine.Printf ("%s path create\t - opens menu for path creation.", self);
             engine.Printf ("%s path delete\t - delete path from cached to nearest waypoint.", self);
             engine.Printf ("%s path create_in\t - creating incoming path connection.", self);
             engine.Printf ("%s path create_out\t - creating outgoing path connection.", self);
             engine.Printf ("%s path create_both\t - creating both-ways path connection.", self);
             engine.Printf ("%s exp save\t - save the experience data.", self);
          }
      }
   }
   else if (_stricmp (arg0, "bot_takedamage") == 0 && !IsNullString (arg1))
   {
      bool isOn = !!(atoi (arg1) == 1);

      for (int i = 0; i < engine.MaxClients (); i++)
      {
         Bot *bot = bots.GetBot (i);

         if (bot != nullptr)
         {
            bot->pev->takedamage = isOn ? 0.0f : 1.0f;
         }
      }
   }

   // displays main bot menu
   else if (_stricmp (arg0, "botmenu") == 0 || _stricmp (arg0, "menu") == 0)
      DisplayMenuToClient (ent, &g_menus[0]);

   // display command menu
   else if (_stricmp (arg0, "cmdmenu") == 0 || _stricmp (arg0, "cmenu") == 0)
   {
      if (IsAlive (ent))
         DisplayMenuToClient (ent, &g_menus[18]);
      else
      {
         DisplayMenuToClient (ent, nullptr); // reset menu display
         engine.CenterPrintf ("You're dead, and have no access to this menu");
      }
   }

   // waypoint manimupulation (really obsolete, can be edited through menu) (supported only on listen server)
   else if (_stricmp (arg0, "waypoint") == 0 || _stricmp (arg0, "wp") == 0 || _stricmp (arg0, "wpt") == 0)
   {
      if (engine.IsDedicatedServer () || engine.IsNullEntity (g_hostEntity))
         return 2;

      // enables or disable waypoint displaying
      if (_stricmp (arg1, "on") == 0)
      {
         g_waypointOn = true;
         engine.Printf ("Waypoint Editing Enabled");

         // enables noclip cheat
         if (_stricmp (arg2, "noclip") == 0)
         {
            if (g_editNoclip)
            {
               g_hostEntity->v.movetype = MOVETYPE_WALK;
               engine.Printf ("Noclip Cheat Disabled");
            }
            else
            {
               g_hostEntity->v.movetype = MOVETYPE_NOCLIP;
               engine.Printf ("Noclip Cheat Enabled");
            }
            g_editNoclip = !g_editNoclip; // switch on/off (XOR it!)
         }
         engine.IssueCmd ("yapb wp mdl on");
      }

      // switching waypoint editing off
      else if (_stricmp (arg1, "off") == 0)
      {
         g_waypointOn = false;
         g_editNoclip = false;
         g_hostEntity->v.movetype = MOVETYPE_WALK;

         engine.Printf ("Waypoint Editing Disabled");
         engine.IssueCmd ("yapb wp mdl off");
      }

      // toggles displaying player models on spawn spots
      else if (_stricmp (arg1, "mdl") == 0 || _stricmp (arg1, "models") == 0)
      {
         if (_stricmp (arg2, "on") == 0)
         {
            EntityIndexer entities (ENTITY_SEARCH_CLASSNAME, "info_player_start");

            while (entities.FindNext ())
               entities.GetCurrentEntity ()->v.effects &= ~EF_NODRAW;

            entities.StartNewSearch (ENTITY_SEARCH_CLASSNAME, "info_player_deathmatch");

            while (entities.FindNext ())
               entities.GetCurrentEntity ()->v.effects &= ~EF_NODRAW;

            entities.StartNewSearch (ENTITY_SEARCH_CLASSNAME, "info_vip_start");

            while (entities.FindNext ())
               entities.GetCurrentEntity ()->v.effects &= ~EF_NODRAW;

            engine.IssueCmd ("mp_roundtime 9"); // reset round time to maximum
            engine.IssueCmd ("mp_timelimit 0"); // disable the time limit
            engine.IssueCmd ("mp_freezetime 0"); // disable freezetime
         }
         else if (_stricmp (arg2, "off") == 0)
         {
            EntityIndexer entities (ENTITY_SEARCH_CLASSNAME, "info_player_start");

            while (entities.FindNext ())
               entities.GetCurrentEntity ()->v.effects |= EF_NODRAW;

            entities.StartNewSearch (ENTITY_SEARCH_CLASSNAME, "info_player_deathmatch");

            while (entities.FindNext ())
               entities.GetCurrentEntity ()->v.effects |= EF_NODRAW;

            entities.StartNewSearch (ENTITY_SEARCH_CLASSNAME, "info_vip_start");

            while (entities.FindNext ())
               entities.GetCurrentEntity ()->v.effects |= EF_NODRAW;
         }
      }

      // show direction to specified waypoint
      else if (_stricmp (arg1, "find") == 0)
         waypoints.SetFindIndex (atoi (arg2));

      // opens adding waypoint menu
      else if (_stricmp (arg1, "add") == 0)
      {
         g_waypointOn = true;  // turn waypoints on
         DisplayMenuToClient (g_hostEntity, &g_menus[12]);
      }

      // creates basic waypoints on the map (ladder/spawn points/goals)
      else if (_stricmp (arg1, "addbasic") == 0)
      {
         waypoints.CreateBasic ();
         engine.CenterPrintf ("Basic waypoints was Created");
      }

      // delete nearest to host edict waypoint
      else if (_stricmp (arg1, "delete") == 0)
      {
         g_waypointOn = true; // turn waypoints on
         waypoints.Delete ();
      }

      // save waypoint data into file on hard disk
      else if (_stricmp (arg1, "save") == 0)
      {
         char *waypointSaveMessage = engine.TraslateMessage ("Waypoints Saved");

         if (_stricmp (arg2, "nocheck") == 0)
         {
            waypoints.Save ();
            engine.Printf (waypointSaveMessage);
         }
         else if (waypoints.NodesValid ())
         {
            waypoints.Save ();
            engine.Printf (waypointSaveMessage);
         }
      }

      // remove waypoint and all corresponding files from hard disk
      else if (_stricmp (arg1, "erase") == 0)
         waypoints.EraseFromHardDisk ();

      // load all waypoints again (overrides all changes, that wasn't saved)
      else if (_stricmp (arg1, "load") == 0)
      {
         if (waypoints.Load ())
            engine.Printf ("Waypoints loaded");
      }

      // check all nodes for validation
      else if (_stricmp (arg1, "check") == 0)
      {
         if (waypoints.NodesValid ())
            engine.CenterPrintf ("Nodes work Fine");
      }

      // opens menu for setting (removing) waypoint flags
      else if (_stricmp (arg1, "flags") == 0)
         DisplayMenuToClient (g_hostEntity, &g_menus[13]);

      // setting waypoint radius
      else if (_stricmp (arg1, "setradius") == 0)
         waypoints.SetRadius (atoi (arg2));

      // remembers nearest waypoint
      else if (_stricmp (arg1, "cache") == 0)
         waypoints.CacheWaypoint ();

      // teleport player to specified waypoint
      else if (_stricmp (arg1, "teleport") == 0)
      {
         int teleportPoint = atoi (arg2);

         if (teleportPoint < g_numWaypoints)
         {
            Path *path = waypoints.GetPath (teleportPoint);

            g_engfuncs.pfnSetOrigin (g_hostEntity, path->origin);
            g_waypointOn = true;

            engine.Printf ("Player '%s' teleported to waypoint #%d (x:%.1f, y:%.1f, z:%.1f)", ValveString::Get (g_hostEntity->v.netname), teleportPoint, path->origin.x, path->origin.y, path->origin.z); //-V807
            g_editNoclip = true;
         }
      }

      // displays waypoint menu
      else if (_stricmp (arg1, "menu") == 0)
         DisplayMenuToClient (g_hostEntity, &g_menus[9]);

      // otherwise display waypoint current status
      else
         engine.Printf ("Waypoints are %s", g_waypointOn == true ? "Enabled" : "Disabled");
   }

   // path waypoint editing system (supported only on listen server)
   else if (_stricmp (arg0, "pathwaypoint") == 0 || _stricmp (arg0, "path") == 0 || _stricmp (arg0, "pwp") == 0)
   {
      if (engine.IsDedicatedServer () || engine.IsNullEntity (g_hostEntity))
         return 2;

      // opens path creation menu
      if (_stricmp (arg1, "create") == 0)
         DisplayMenuToClient (g_hostEntity, &g_menus[20]);

      // creates incoming path from the cached waypoint
      else if (_stricmp (arg1, "create_in") == 0)
         waypoints.CreatePath (CONNECTION_INCOMING);

      // creates outgoing path from current waypoint
      else if (_stricmp (arg1, "create_out") == 0)
         waypoints.CreatePath (CONNECTION_OUTGOING);

      // creates bidirectional path from cahed to current waypoint
      else if (_stricmp (arg1, "create_both") == 0)
         waypoints.CreatePath (CONNECTION_BOTHWAYS);

      // delete special path
      else if (_stricmp (arg1, "delete") == 0)
         waypoints.DeletePath ();

      // sets auto path maximum distance
      else if (_stricmp (arg1, "autodistance") == 0)
         DisplayMenuToClient (g_hostEntity, &g_menus[19]);
   }

   // automatic waypoint handling (supported only on listen server)
   else if (_stricmp (arg0, "autowaypoint") == 0 || _stricmp (arg0, "autowp") == 0)
   {
      if (engine.IsDedicatedServer () || engine.IsNullEntity (g_hostEntity))
         return 2;

      // enable autowaypointing
      if (_stricmp (arg1, "on") == 0)
      {
         g_autoWaypoint = true;
         g_waypointOn = true; // turn this on just in case
      }

      // disable autowaypointing
      else if (_stricmp (arg1, "off") == 0)
         g_autoWaypoint = false;

      // display status
      engine.Printf ("Auto-Waypoint %s", g_autoWaypoint ? "Enabled" : "Disabled");
   }

   // experience system handling (supported only on listen server)
   else if (_stricmp (arg0, "experience") == 0 || _stricmp (arg0, "exp") == 0)
   {
      if (engine.IsDedicatedServer () || engine.IsNullEntity (g_hostEntity))
         return 2;

      // write experience table (and visibility table) to hard disk
      if (_stricmp (arg1, "save") == 0)
      {
         waypoints.SaveExperienceTab ();
         waypoints.SaveVisibilityTab ();

         engine.Printf ("Experience tab saved");
      }
   }
   else
      return 0; // command is not handled by bot

   return 1; // command was handled by bot
}

void CommandHandler (void)
{
   // this function is the dedicated server command handler for the new yapb server command we
   // registered at game start. It will be called by the engine each time a server command that
   // starts with "yapb" is entered in the server console. It works exactly the same way as
   // ClientCommand() does, using the CmdArgc() and CmdArgv() facilities of the engine. Argv(0)
   // is the server command itself (here "yapb") and the next ones are its arguments. Just like
   // the stdio command-line parsing in C when you write "long main (long argc, char **argv)".

   // check status for dedicated server command
   if (BotCommandHandler (g_hostEntity, IsNullString (g_engfuncs.pfnCmd_Argv (1)) ? "help" : g_engfuncs.pfnCmd_Argv (1), g_engfuncs.pfnCmd_Argv (2), g_engfuncs.pfnCmd_Argv (3), g_engfuncs.pfnCmd_Argv (4), g_engfuncs.pfnCmd_Argv (5), g_engfuncs.pfnCmd_Argv (6), g_engfuncs.pfnCmd_Argv (0)) == 0)
      engine.Printf ("Unknown command: %s", g_engfuncs.pfnCmd_Argv (1));
}

void ParseVoiceEvent (const String &base, int type, float timeToRepeat)
{
   // this function does common work of parsing single line of voice chatter

   Array <String> temp = String (base).Split (',');
   ChatterItem chatterItem;

   FOR_EACH_AE (temp, i)
   {
      temp[i].Trim ().TrimQuotes ();

      if (engine.GetWaveLength (temp[i]) <= 0.0f)
         continue;

      chatterItem.name = temp[i];
      chatterItem.repeatTime = timeToRepeat;

      g_chatterFactory[type].Push (chatterItem);
    }
   temp.RemoveAll ();
}

// forwards for MemoryFile
MemoryFile::MF_Loader MemoryFile::Loader = nullptr;
MemoryFile::MF_Unloader MemoryFile::Unloader = nullptr;

void InitConfig (void)
{
   if (!MemoryFile::Loader && !MemoryFile::Unloader)
   {
      MemoryFile::Loader = reinterpret_cast <MemoryFile::MF_Loader> (g_engfuncs.pfnLoadFileForMe);
      MemoryFile::Unloader = reinterpret_cast <MemoryFile::MF_Unloader> (g_engfuncs.pfnFreeFile);
   }

   MemoryFile fp;
   char line[512];

   KeywordFactory replyKey;

   // fixes for crashing if configs couldn't be accessed
   g_chatFactory.SetSize (CHAT_TOTAL);
   g_chatterFactory.SetSize (Chatter_Total);

   #define SKIP_COMMENTS() if (line[0] == '/' || line[0] == '\r' || line[0] == '\n' || line[0] == 0 || line[0] == ' ' || line[0] == '\t' || line[0] == ';') continue

   // NAMING SYSTEM INITIALIZATION
   if (OpenConfig ("names.cfg", "Name configuration file not found.", &fp , true))
   {
      g_botNames.RemoveAll ();

      while (fp.GetBuffer (line, 255))
      {
         SKIP_COMMENTS ();

         Array <String> pair = String (line).Split ("\t\t");

         if (pair.GetElementNumber () > 1)
            strncpy (line, pair[0].Trim ().GetBuffer (), SIZEOF_CHAR (line));

         String::TrimExternalBuffer (line);
         line[32] = 0;

         BotName item;
         memset (&item, 0, sizeof (item));

         item.name = line;
         item.used = false;

         if (pair.GetElementNumber () > 1)
            item.steamId = pair[1].Trim ();

         g_botNames.Push (item);
      }
      fp.Close ();
   }

   // CHAT SYSTEM CONFIG INITIALIZATION
   if (OpenConfig ("chat.cfg", "Chat file not found.", &fp, true))
   {
      char section[80];
      int chatType = -1;

      while (fp.GetBuffer (line, 255))
      {
         SKIP_COMMENTS ();
         strncpy (section, Engine::ExtractSingleField (line, 0, 1), SIZEOF_CHAR (section));

         if (strcmp (section, "[KILLED]") == 0)
         {
            chatType = 0;
            continue;
         }
         else if (strcmp (section, "[BOMBPLANT]") == 0)
         {
            chatType = 1;
            continue;
         }
         else if (strcmp (section, "[DEADCHAT]") == 0)
         {
            chatType = 2;
            continue;
         }
         else if (strcmp (section, "[REPLIES]") == 0)
         {
            chatType = 3;
            continue;
         }
         else if (strcmp (section, "[UNKNOWN]") == 0)
         {
            chatType = 4;
            continue;
         }
         else if (strcmp (section, "[TEAMATTACK]") == 0)
         {
            chatType = 5;
            continue;
         }
         else if (strcmp (section, "[WELCOME]") == 0)
         {
            chatType = 6;
            continue;
         }
         else if (strcmp (section, "[TEAMKILL]") == 0)
         {
            chatType = 7;
            continue;
         }

         if (chatType != 3)
            line[79] = 0;

         String::TrimExternalBuffer (line);

         switch (chatType)
         {
         case 0:
            g_chatFactory[CHAT_KILLING].Push (line);
            break;

         case 1:
            g_chatFactory[CHAT_BOMBPLANT].Push (line);
            break;

         case 2:
            g_chatFactory[CHAT_DEAD].Push (line);
            break;

         case 3:
            if (strstr (line, "@KEY") != nullptr)
            {
               if (!replyKey.keywords.IsEmpty () && !replyKey.replies.IsEmpty ())
               {
                  g_replyFactory.Push (replyKey);
                  replyKey.replies.RemoveAll ();
               }

               replyKey.keywords.RemoveAll ();
               replyKey.keywords = String (&line[4]).Split (',');

               FOR_EACH_AE (replyKey.keywords, i)
                  replyKey.keywords[i].Trim ().TrimQuotes ();
            }
            else if (!replyKey.keywords.IsEmpty ())
               replyKey.replies.Push (line);

            break;

         case 4:
            g_chatFactory[CHAT_NOKW].Push (line);
            break;

         case 5:
            g_chatFactory[CHAT_TEAMATTACK].Push (line);
            break;

         case 6:
            g_chatFactory[CHAT_WELCOME].Push (line);
            break;

         case 7:
            g_chatFactory[CHAT_TEAMKILL].Push (line);
            break;
         }
      }
      fp.Close ();
   }
   else
   {
      extern ConVar yb_chat;
      yb_chat.SetInt (0);
   }
  
   // GENERAL DATA INITIALIZATION
   if (OpenConfig ("general.cfg", "General configuration file not found. Loading defaults", &fp))
   {
      while (fp.GetBuffer (line, 255))
      {
         SKIP_COMMENTS ();

         Array <String> pair = String (line).Split ('=');

         if (pair.GetElementNumber () != 2)
            continue;

         pair[0].Trim ().Trim ();
         pair[1].Trim ().Trim ();

         Array <String> splitted = pair[1].Split (',');

         if (pair[0] == "MapStandard")
         {
            if (splitted.GetElementNumber () != NUM_WEAPONS)
               AddLogEntry (true, LL_FATAL, "%s entry in general config is not valid.", pair[0].GetBuffer ());

            for (int i = 0; i < NUM_WEAPONS; i++)
               g_weaponSelect[i].teamStandard = splitted[i].ToInt ();
         }
         else if (pair[0] == "MapAS")
         {
            if (splitted.GetElementNumber () != NUM_WEAPONS)
               AddLogEntry (true, LL_FATAL, "%s entry in general config is not valid.", pair[0].GetBuffer ());

            for (int i = 0; i < NUM_WEAPONS; i++)
               g_weaponSelect[i].teamAS = splitted[i].ToInt ();
         }
         else if (pair[0] == "GrenadePercent")
         {
            if (splitted.GetElementNumber () != 3)
               AddLogEntry (true, LL_FATAL, "%s entry in general config is not valid.", pair[0].GetBuffer ());

            for (int i = 0; i < 3; i++)
               g_grenadeBuyPrecent[i] = splitted[i].ToInt ();
         }
         else if (pair[0] == "Economics")
         {
            if (splitted.GetElementNumber () != 11)
               AddLogEntry (true, LL_FATAL, "%s entry in general config is not valid.", pair[0].GetBuffer ());

            for (int i = 0; i < 11; i++)
               g_botBuyEconomyTable[i] = splitted[i].ToInt ();
         }
         else if (pair[0] == "PersonalityNormal")
         {
            if (splitted.GetElementNumber () != NUM_WEAPONS)
               AddLogEntry (true, LL_FATAL, "%s entry in general config is not valid.", pair[0].GetBuffer ());

            for (int i = 0; i < NUM_WEAPONS; i++)
               g_normalWeaponPrefs[i] = splitted[i].ToInt ();
         }
         else if (pair[0] == "PersonalityRusher")
         {
            if (splitted.GetElementNumber () != NUM_WEAPONS)
               AddLogEntry (true, LL_FATAL, "%s entry in general config is not valid.", pair[0].GetBuffer ());

            for (int i = 0; i < NUM_WEAPONS; i++)
               g_rusherWeaponPrefs[i] = splitted[i].ToInt ();
         }
         else if (pair[0] == "PersonalityCareful")
         {
            if (splitted.GetElementNumber () != NUM_WEAPONS)
               AddLogEntry (true, LL_FATAL, "%s entry in general config is not valid.", pair[0].GetBuffer ());

            for (int i = 0; i < NUM_WEAPONS; i++)
               g_carefulWeaponPrefs[i] = splitted[i].ToInt ();
         }
      }
      fp.Close ();
   }

   // CHATTER SYSTEM INITIALIZATION
   if (OpenConfig ("chatter.cfg", "Couldn't open chatter system configuration", &fp) && !(g_gameFlags & GAME_LEGACY) && yb_communication_type.GetInt () == 2)
   {
      Array <String> array;

      extern ConVar yb_chatter_path;

      while (fp.GetBuffer (line, 511))
      {
         SKIP_COMMENTS ();

         if (strncmp (line, "RewritePath", 11) == 0)
            yb_chatter_path.SetString (String (&line[12]).Trim ());
         else if (strncmp (line, "Event", 5) == 0)
         {
            array = String (&line[6]).Split ('=');

            if (array.GetElementNumber () != 2)
               AddLogEntry (true, LL_FATAL, "Error in chatter config file syntax... Please correct all Errors.");

            FOR_EACH_AE (array, i)
               array[i].Trim ().Trim (); // double trim

            // just to be more unique :)
            array[1].TrimLeft ('(');
            array[1].TrimRight (';');
            array[1].TrimRight (')');

            #define PARSE_CHATTER_ITEM(type, timeToRepeatAgain) { if (strcmp (array[0], #type) == 0) ParseVoiceEvent (array[1], type, timeToRepeatAgain); }
            #define PARSE_CHATTER_ITEM_NR(type) PARSE_CHATTER_ITEM(type, 99999.0f)

            // radio system
            PARSE_CHATTER_ITEM_NR (Radio_CoverMe);
            PARSE_CHATTER_ITEM_NR (Radio_YouTakePoint);
            PARSE_CHATTER_ITEM_NR (Radio_HoldPosition);
            PARSE_CHATTER_ITEM_NR (Radio_RegroupTeam);
            PARSE_CHATTER_ITEM_NR (Radio_FollowMe);
            PARSE_CHATTER_ITEM_NR (Radio_TakingFire);
            PARSE_CHATTER_ITEM_NR (Radio_GoGoGo);
            PARSE_CHATTER_ITEM_NR (Radio_Fallback);
            PARSE_CHATTER_ITEM_NR (Radio_StickTogether);
            PARSE_CHATTER_ITEM_NR (Radio_GetInPosition);
            PARSE_CHATTER_ITEM_NR (Radio_StormTheFront);
            PARSE_CHATTER_ITEM_NR (Radio_ReportTeam);
            PARSE_CHATTER_ITEM_NR (Radio_Affirmative);
            PARSE_CHATTER_ITEM_NR (Radio_EnemySpotted);
            PARSE_CHATTER_ITEM_NR (Radio_NeedBackup);
            PARSE_CHATTER_ITEM_NR (Radio_SectorClear);
            PARSE_CHATTER_ITEM_NR (Radio_InPosition);
            PARSE_CHATTER_ITEM_NR (Radio_ReportingIn);
            PARSE_CHATTER_ITEM_NR (Radio_ShesGonnaBlow);
            PARSE_CHATTER_ITEM_NR (Radio_Negative);
            PARSE_CHATTER_ITEM_NR (Radio_EnemyDown);

            // voice system
            PARSE_CHATTER_ITEM (Chatter_SpotTheBomber, 4.3f);
            PARSE_CHATTER_ITEM (Chatter_VIPSpotted, 5.3f);
            PARSE_CHATTER_ITEM (Chatter_FriendlyFire, 2.1f);
            PARSE_CHATTER_ITEM_NR (Chatter_DiePain);
            PARSE_CHATTER_ITEM (Chatter_GotBlinded, 5.0f);
            PARSE_CHATTER_ITEM_NR (Chatter_GoingToPlantBomb);
            PARSE_CHATTER_ITEM_NR (Chatter_GoingToGuardVIPSafety);
            PARSE_CHATTER_ITEM_NR (Chatter_RescuingHostages);
            PARSE_CHATTER_ITEM_NR (Chatter_GoingToCamp);
            PARSE_CHATTER_ITEM_NR (Chatter_TeamKill);
            PARSE_CHATTER_ITEM_NR (Chatter_ReportingIn);
            PARSE_CHATTER_ITEM (Chatter_GuardDroppedC4, 3.0f);
            PARSE_CHATTER_ITEM_NR (Chatter_Camp);
            PARSE_CHATTER_ITEM_NR (Chatter_GuardingVipSafety);
            PARSE_CHATTER_ITEM_NR (Chatter_PlantingC4);
            PARSE_CHATTER_ITEM (Chatter_DefusingC4, 3.0f);
            PARSE_CHATTER_ITEM_NR (Chatter_InCombat);
            PARSE_CHATTER_ITEM_NR (Chatter_SeeksEnemy);
            PARSE_CHATTER_ITEM_NR (Chatter_Nothing);
            PARSE_CHATTER_ITEM_NR (Chatter_EnemyDown);
            PARSE_CHATTER_ITEM_NR (Chatter_UseHostage);
            PARSE_CHATTER_ITEM (Chatter_FoundC4, 5.5f);
            PARSE_CHATTER_ITEM_NR (Chatter_WonTheRound);
            PARSE_CHATTER_ITEM (Chatter_ScaredEmotion, 6.1f);
            PARSE_CHATTER_ITEM (Chatter_HeardEnemy, 12.2f);
            PARSE_CHATTER_ITEM (Chatter_SniperWarning, 4.3f);
            PARSE_CHATTER_ITEM (Chatter_SniperKilled, 2.1f);
            PARSE_CHATTER_ITEM_NR (Chatter_QuicklyWonTheRound);
            PARSE_CHATTER_ITEM (Chatter_OneEnemyLeft, 2.5f);
            PARSE_CHATTER_ITEM (Chatter_TwoEnemiesLeft, 2.5f);
            PARSE_CHATTER_ITEM (Chatter_ThreeEnemiesLeft, 2.5f);
            PARSE_CHATTER_ITEM_NR (Chatter_NoEnemiesLeft);
            PARSE_CHATTER_ITEM_NR (Chatter_FoundBombPlace);
            PARSE_CHATTER_ITEM_NR (Chatter_WhereIsTheBomb);
            PARSE_CHATTER_ITEM_NR (Chatter_DefendingBombSite);
            PARSE_CHATTER_ITEM_NR (Chatter_BarelyDefused);
            PARSE_CHATTER_ITEM_NR (Chatter_NiceshotCommander);
            PARSE_CHATTER_ITEM (Chatter_NiceshotPall, 2.0);
            PARSE_CHATTER_ITEM (Chatter_GoingToGuardHostages, 3.0f);
            PARSE_CHATTER_ITEM (Chatter_GoingToGuardDoppedBomb, 3.0f);
            PARSE_CHATTER_ITEM (Chatter_OnMyWay, 1.5f);
            PARSE_CHATTER_ITEM (Chatter_LeadOnSir, 5.0f);
            PARSE_CHATTER_ITEM (Chatter_Pinned_Down, 5.0f);
            PARSE_CHATTER_ITEM (Chatter_GottaFindTheBomb, 3.0f);
            PARSE_CHATTER_ITEM (Chatter_You_Heard_The_Man, 3.0f);
            PARSE_CHATTER_ITEM (Chatter_Lost_The_Commander, 4.5f);
            PARSE_CHATTER_ITEM (Chatter_NewRound, 3.5f);
            PARSE_CHATTER_ITEM (Chatter_CoverMe, 3.5f);
            PARSE_CHATTER_ITEM (Chatter_BehindSmoke, 3.5f);
            PARSE_CHATTER_ITEM (Chatter_BombSiteSecured, 3.5f);
         }
      }
      fp.Close ();
   }
   else
   {
      yb_communication_type.SetInt (1);
      AddLogEntry (true, LL_DEFAULT, "Chatter Communication disabled.");
   }

   // LOCALIZER INITITALIZATION
   if (OpenConfig ("lang.cfg", "Specified language not found", &fp, true) && !(g_gameFlags & GAME_LEGACY))
   {
      if (engine.IsDedicatedServer ())
         return; // dedicated server will use only english translation

      enum Lang { Lang_Original, Lang_Translate } langState = static_cast <Lang> (2);

      char buffer[1024];
      TranslatorPair temp = {"", ""};

      while (fp.GetBuffer (line, 255))
      {
         if (strncmp (line, "[ORIGINAL]", 10) == 0)
         {
            langState = Lang_Original;

            if (!IsNullString (buffer))
            {
               String::TrimExternalBuffer (buffer);
               temp.translated = _strdup (buffer);
               buffer[0] = 0x0;
            }

            if (!IsNullString (temp.translated) && !IsNullString (temp.original))
               engine.PushTranslationPair (temp);
         }
         else if (strncmp (line, "[TRANSLATED]", 12) == 0)
         {
            String::TrimExternalBuffer (buffer);
            temp.original = _strdup (buffer);
            buffer[0] = 0x0;

            langState = Lang_Translate;
         }
         else
         {
            switch (langState)
            {
            case Lang_Original:
               strncat (buffer, line, 1024 - 1 - strlen (buffer));
               break;

            case Lang_Translate:
               strncat (buffer, line, 1024 - 1 - strlen (buffer));
               break;
            }
         }
      }
      fp.Close ();
   }
   else if (g_gameFlags & GAME_LEGACY)
      AddLogEntry (true, LL_DEFAULT, "Multilingual system disabled, due to your Counter-Strike Version!");
   else if (strcmp (yb_language.GetString (), "en") != 0)
      AddLogEntry (true, LL_ERROR, "Couldn't load language configuration");

   // set personality weapon pointers here
   g_weaponPrefs[PERSONALITY_NORMAL] = reinterpret_cast <int *> (&g_normalWeaponPrefs);
   g_weaponPrefs[PERSONALITY_RUSHER] = reinterpret_cast <int *> (&g_rusherWeaponPrefs);
   g_weaponPrefs[PERSONALITY_CAREFUL] = reinterpret_cast <int *> (&g_carefulWeaponPrefs);

   g_timePerSecondUpdate = 0.0f;
}




void GameEventsListener::OnClientConnected (edict_t *ent, const char *addr)
{
   if (strcmp (addr, "loopback") == 0)
      g_hostEntity = ent;

   bots.AdjustQuota (true, ent);
}

void GameEventsListener::OnClientDisconnect (edict_t *ent)
{
   bots.AdjustQuota (false, ent);

   int i = engine.IndexOfEntity (ent) - 1;

   BOT_ASSERT (i >= 0 && i < MAX_ENGINE_PLAYERS);

   Bot *bot = bots.GetBot (i);

   // check if its a bot
   if (bot != nullptr)
   {
      if (bot->pev == &ent->v)
      {
         bot->SwitchChatterIcon (false);
         bot->ReleaseUsedName ();

         bots.Free (i);
      }
   }
}

void GameEventsListener::OnClientUserInfoChanged (edict_t *ent, char *infobuffer)
{
   if (engine.IsDedicatedServer () && !IsValidBot (ent))
   {
      const char *passwordField = yb_password_key.GetString ();
      const char *password = yb_password.GetString ();

      if (!IsNullString (passwordField) || !IsNullString (password))
      {
         int clientIndex = engine.IndexOfEntity (ent) - 1;

         if (strcmp (password, g_engfuncs.pfnInfoKeyValue (infobuffer, const_cast <char *> (passwordField))) == 0)
            g_clients[clientIndex].flags |= CF_ADMIN;
         else
            g_clients[clientIndex].flags &= ~CF_ADMIN;
      }
   }
}

void GameEventsListener::OnGameInit (void)
{
   DetectCSVersion ();
   {
      // print game detection info
      String gameVersionStr;

      if (g_gameFlags & GAME_LEGACY)
         gameVersionStr.Assign ("Legacy");

      else if (g_gameFlags & GAME_CZERO)
         gameVersionStr.Assign ("Condition Zero");

      else if (g_gameFlags & GAME_CSTRIKE16)
         gameVersionStr.Assign ("v1.6");

      if (g_gameFlags & GAME_XASH)
      {
         gameVersionStr.Append (" @ Xash3D Engine");

         if (g_gameFlags & GAME_MOBILITY)
            gameVersionStr.Append (" Mobile");

         gameVersionStr.Replace ("Legacy", "1.6 Limited");
      }
      engine.Printf ("YaPB Bot has detect game version as Counter-Strike: %s", gameVersionStr.GetBuffer ());
   }

   // register server command(s)
   engine.RegisterCmd ("yapb", CommandHandler);
   engine.RegisterCmd ("yb", CommandHandler);

   // execute main config
   engine.IssueCmd ("exec addons/yapb/conf/yapb.cfg");

   // set correct version string
   yb_version.SetString (FormatBuffer ("%d.%d.%d.%u", PRODUCT_VERSION_DWORD_INTERNAL, GenerateBuildNumber ()));

   // register fake metamod command handler if we not! under mm
   if (!gs.IsMetamod ())
   {
      engine.RegisterCmd ("meta", [] (void)
      {
         engine.Printf ("You're launched standalone version of yapb. Metamod is not installed or not enabled!");
      });
   }
}

void GameEventsListener::OnEntitiesTouch (edict_t *ent1, edict_t *ent2)
{
   if (!engine.IsNullEntity (ent2) && (ent2->v.flags & FL_FAKECLIENT))
   {
      Bot *bot = bots.GetBot (ent2);

      if (bot != nullptr)
         bot->VerifyBreakable (ent1);
   }
}

void GameEventsListener::OnEntitySpawned (edict_t *ent)
{
   // for faster access
   const char *entityClassname = ValveString::Get (ent->v.classname);

   if (strcmp (entityClassname, "worldspawn") == 0)
   {
      engine.Precache (ent);
      engine.PushRegisteredConVarsToEngine (true);

      g_engfuncs.pfnPrecacheSound (ValveString::Alloc ("weapons/xbow_hit1.wav")); // waypoint add
      g_engfuncs.pfnPrecacheSound (ValveString::Alloc ("weapons/mine_activate.wav")); // waypoint delete
      g_engfuncs.pfnPrecacheSound (ValveString::Alloc ("common/wpn_hudoff.wav")); // path add/delete start
      g_engfuncs.pfnPrecacheSound (ValveString::Alloc ("common/wpn_hudon.wav")); // path add/delete done
      g_engfuncs.pfnPrecacheSound (ValveString::Alloc ("common/wpn_moveselect.wav")); // path add/delete cancel
      g_engfuncs.pfnPrecacheSound (ValveString::Alloc ("common/wpn_denyselect.wav")); // path add/delete error

      RoundInit ();
      g_mapType = 0; // reset map type as worldspawn is the first entity spawned

       // detect official csbots here, as they causing crash in linkent code when active for some reason
      if (!(g_gameFlags & GAME_LEGACY) && g_engfuncs.pfnCVarGetPointer ("bot_stop") != nullptr)
         g_gameFlags |= GAME_OFFICIAL_CSBOT;
   }

#ifndef XASH_CSDM
   else if (strcmp (entityClassname, "info_player_start") == 0)
   {
      g_engfuncs.pfnSetModel (ent, ValveString::Alloc ("models/player/urban/urban.mdl"));

      ent->v.rendermode = kRenderTransAlpha; // set its render mode to transparency
      ent->v.renderamt = 127; // set its transparency amount
      ent->v.effects |= EF_NODRAW;
   }
   else if (strcmp (entityClassname, "info_player_deathmatch") == 0)
   {
      g_engfuncs.pfnSetModel (ent, ValveString::Alloc ("models/player/terror/terror.mdl"));

      ent->v.rendermode = kRenderTransAlpha; // set its render mode to transparency
      ent->v.renderamt = 127; // set its transparency amount
      ent->v.effects |= EF_NODRAW;
   }

   else if (strcmp (entityClassname, "info_vip_start") == 0)
   {
      g_engfuncs.pfnSetModel (ent, ValveString::Alloc ("models/player/vip/vip.mdl"));

      ent->v.rendermode = kRenderTransAlpha; // set its render mode to transparency
      ent->v.renderamt = 127; // set its transparency amount
      ent->v.effects |= EF_NODRAW;
   }
#endif
   else if (strcmp (entityClassname, "func_vip_safetyzone") == 0 || strcmp (ValveString::Get (ent->v.classname), "info_vip_safetyzone") == 0)
      g_mapType |= MAP_AS; // assassination map

   else if (strcmp (entityClassname, "hostage_entity") == 0)
      g_mapType |= MAP_CS; // rescue map

   else if (strcmp (entityClassname, "func_bomb_target") == 0 || strcmp (ValveString::Get (ent->v.classname), "info_bomb_target") == 0)
      g_mapType |= MAP_DE; // defusion map

   else if (strcmp (entityClassname, "func_escapezone") == 0)
      g_mapType |= MAP_ES;

   // next maps doesn't have map-specific entities, so determine it by name
   else if (strncmp (engine.GetMapName (), "fy_", 3) == 0) // fun map
      g_mapType |= MAP_FY;
   else if (strncmp (engine.GetMapName (), "ka_", 3) == 0) // knife arena map
      g_mapType |= MAP_KA;
}

void GameEventsListener::OnUpdateClientData (edict_t *ent)
{
   extern ConVar yb_latency_display;

   if (yb_latency_display.GetInt () == 2)
      bots.SendPingDataOffsets (ent);
}

void GameEventsListener::OnEngineFrame (bool isPost)
{
   // run bot's ai only after game logic
   if (isPost)
   {
      bots.Think ();
      return;
   }

   // run periodic update of bot states
   bots.PeriodicThink ();

   // record some stats of all players on the server
   for (int i = 0; i < engine.MaxClients (); i++)
   {
      edict_t *player = engine.EntityOfIndex (i + 1);

      if (!engine.IsNullEntity (player) && (player->v.flags & FL_CLIENT))
      {
         g_clients[i].ent = player;
         g_clients[i].flags |= CF_USED;

         if (IsAlive (player))
            g_clients[i].flags |= CF_ALIVE;
         else
            g_clients[i].flags &= ~CF_ALIVE;

         if (g_clients[i].flags & CF_ALIVE)
         {
            // keep the clipping mode enabled, or it can be turned off after new round has started
            if (g_hostEntity == player && g_editNoclip)
               g_hostEntity->v.movetype = MOVETYPE_NOCLIP;

            g_clients[i].origin = player->v.origin;
            SoundSimulateUpdate (i);
         }
      }
      else
      {
         g_clients[i].flags &= ~(CF_USED | CF_ALIVE);
         g_clients[i].ent = nullptr;
      }
   }

   if (!engine.IsDedicatedServer () && !engine.IsNullEntity (g_hostEntity))
   {
      if (g_waypointOn)
         waypoints.Think ();

      CheckWelcomeMessage ();
   }
   bots.SetDeathMsgState (false);

   if (g_timePerSecondUpdate < engine.Time ())
   {
      for (int i = 0; i < engine.MaxClients (); i++)
      {
         edict_t *player = engine.EntityOfIndex (i + 1);

         // code below is executed only on dedicated server
         if (engine.IsDedicatedServer () && !engine.IsNullEntity (player) && (player->v.flags & FL_CLIENT) && !(player->v.flags & FL_FAKECLIENT))
         {
            Client &client = g_clients[i];

            if (client.flags & CF_ADMIN)
            {
               if (IsNullString (yb_password_key.GetString ()) && IsNullString (yb_password.GetString ()))
                  client.flags &= ~CF_ADMIN;
               else if (strcmp (yb_password.GetString (), g_engfuncs.pfnInfoKeyValue (g_engfuncs.pfnGetInfoKeyBuffer (client.ent), const_cast <char *> (yb_password_key.GetString ()))))
               {
                  client.flags &= ~CF_ADMIN;
                  engine.Printf ("Player %s had lost remote access to yapb.", ValveString::Get (player->v.netname));
               }
            }
            else if (!(client.flags & CF_ADMIN) && !IsNullString (yb_password_key.GetString ()) && !IsNullString (yb_password.GetString ()))
            {
               if (strcmp (yb_password.GetString (), g_engfuncs.pfnInfoKeyValue (g_engfuncs.pfnGetInfoKeyBuffer (client.ent), const_cast <char *> (yb_password_key.GetString ()))) == 0)
               {
                  client.flags |= CF_ADMIN;
                  engine.Printf ("Player %s had gained full remote access to yapb.", ValveString::Get (player->v.netname));
               }
            }
         }
      }
      bots.CalculatePingOffsets ();

      if (gs.IsMetamod ())
      {
         static cvar_t *csdm_active;
         static cvar_t *mp_freeforall;

         if (csdm_active == nullptr)
            csdm_active = g_engfuncs.pfnCVarGetPointer ("csdm_active");

         if (mp_freeforall == nullptr)
            mp_freeforall = g_engfuncs.pfnCVarGetPointer ("mp_freeforall");

         if (csdm_active != nullptr && csdm_active->value > 0)
            yb_csdm_mode.SetInt (mp_freeforall != nullptr && mp_freeforall->value > 0 ? 2 : 1);
      }
      g_timePerSecondUpdate = engine.Time () + 1.0f;
   }
   
   // update active grenades all around
   bots.UpdateActiveGrenades ();

   // keep bot number up to date
   bots.MaintainBotQuota ();
}

void GameEventsListener::OnServerActivated (bool isPost)
{
   if (isPost)
   {
      waypoints.InitializeVisibility ();
      return;
   }
   FreeBotMemory ();
   InitConfig (); // initialize all config files

   // do level initialization stuff here...
   waypoints.Init ();
   waypoints.Load ();

   // execute main config
   engine.IssueCmd ("exec addons/yapb/conf/yapb.cfg");

   if (File::Accessible (FormatBuffer ("%s/maps/%s_yapb.cfg", engine.GetModName (), engine.GetMapName ())))
   {
      engine.IssueCmd ("exec maps/%s_yapb.cfg", engine.GetMapName ());
      engine.Printf ("Executing Map-Specific config file");
   }
   bots.InitQuota ();
}

void GameEventsListener::OnServerDeactivated (void)
{
   // save collected experience on shutdown
   waypoints.SaveExperienceTab ();
   waypoints.SaveVisibilityTab ();

   FreeBotMemory ();
}

bool GameEventsListener::OnClientCommand (edict_t *ent, const char *command, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6)
{
   static int fillServerTeam = 5;
   static bool fillCommand = false;

   if (!engine.IsBotCommand () && (ent == g_hostEntity || (g_clients[engine.IndexOfEntity (ent) - 1].flags & CF_ADMIN)))
   {
      if (_stricmp (command, "yapb") == 0 || _stricmp (command, "yb") == 0)
      {
         int state = BotCommandHandler (ent, IsNullString (arg1) ? "help" : arg1, arg2, arg3, arg4, arg5, arg6, command);

         switch (state)
         {
         case 0:
         engine.ClientPrintf (ent, "Unknown command: %s", arg1);
         break;

         case 2:
         engine.ClientPrintf (ent, "Command %s, can only be executed from server console.", arg1);
         break;
         }
         return true;
      }
      else if (_stricmp (command, "menuselect") == 0 && !IsNullString (arg1) && g_clients[engine.IndexOfEntity (ent) - 1].menu != nullptr)
      {
         Client *client = &g_clients[engine.IndexOfEntity (ent) - 1];
         int selection = atoi (arg1);

         if (client->menu == &g_menus[12])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            waypoints.Add (selection - 1);
            break;

            case 8:
            waypoints.Add (100);
            break;

            case 9:
            waypoints.SetLearnJumpWaypoint ();
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[13])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            waypoints.ToggleFlags (FLAG_NOHOSTAGE);
            break;

            case 2:
            waypoints.ToggleFlags (FLAG_TF_ONLY);
            break;

            case 3:
            waypoints.ToggleFlags (FLAG_CF_ONLY);
            break;

            case 4:
            waypoints.ToggleFlags (FLAG_LIFT);
            break;

            case 5:
            waypoints.ToggleFlags (FLAG_SNIPER);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[9])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            if (g_waypointOn)
               engine.IssueCmd ("yapb waypoint off");
            else
               engine.IssueCmd ("yapb waypoint on");
            break;

            case 2:
            g_waypointOn = true;
            waypoints.CacheWaypoint ();
            break;

            case 3:
            g_waypointOn = true;
            DisplayMenuToClient (ent, &g_menus[20]);
            break;

            case 4:
            g_waypointOn = true;
            waypoints.DeletePath ();
            break;

            case 5:
            g_waypointOn = true;
            DisplayMenuToClient (ent, &g_menus[12]);
            break;

            case 6:
            g_waypointOn = true;
            waypoints.Delete ();
            break;

            case 7:
            g_waypointOn = true;
            DisplayMenuToClient (ent, &g_menus[19]);
            break;

            case 8:
            g_waypointOn = true;
            DisplayMenuToClient (ent, &g_menus[11]);
            break;

            case 9:
            DisplayMenuToClient (ent, &g_menus[10]);
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[10])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            {
               int terrPoints = 0;
               int ctPoints = 0;
               int goalPoints = 0;
               int rescuePoints = 0;
               int campPoints = 0;
               int sniperPoints = 0;
               int noHostagePoints = 0;

               for (int i = 0; i < g_numWaypoints; i++)
               {
                  Path *path = waypoints.GetPath (i);

                  if (path->flags & FLAG_TF_ONLY)
                     terrPoints++;

                  if (path->flags & FLAG_CF_ONLY)
                     ctPoints++;

                  if (path->flags & FLAG_GOAL)
                     goalPoints++;

                  if (path->flags & FLAG_RESCUE)
                     rescuePoints++;

                  if (path->flags & FLAG_CAMP)
                     campPoints++;

                  if (path->flags & FLAG_SNIPER)
                     sniperPoints++;

                  if (path->flags & FLAG_NOHOSTAGE)
                     noHostagePoints++;
               }
               engine.Printf ("Waypoints: %d - T Points: %d\n"
                  "CT Points: %d - Goal Points: %d\n"
                  "Rescue Points: %d - Camp Points: %d\n"
                  "Block Hostage Points: %d - Sniper Points: %d\n", g_numWaypoints, terrPoints, ctPoints, goalPoints, rescuePoints, campPoints, noHostagePoints, sniperPoints);
            }
            break;

            case 2:
            g_waypointOn = true;
            g_autoWaypoint &= 1;
            g_autoWaypoint ^= 1;

            engine.CenterPrintf ("Auto-Waypoint %s", g_autoWaypoint ? "Enabled" : "Disabled");
            break;

            case 3:
            g_waypointOn = true;
            DisplayMenuToClient (ent, &g_menus[13]);
            break;

            case 4:
            if (waypoints.NodesValid ())
               waypoints.Save ();
            else
               engine.CenterPrintf ("Waypoint not saved\nThere are errors, see console");
            break;

            case 5:
            waypoints.Save ();
            break;

            case 6:
            waypoints.Load ();
            break;

            case 7:
            if (waypoints.NodesValid ())
               engine.CenterPrintf ("Nodes works fine");
            else
               engine.CenterPrintf ("There are errors, see console");
            break;

            case 8:
            engine.IssueCmd ("yapb wp on noclip");
            break;

            case 9:
            DisplayMenuToClient (ent, &g_menus[9]);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[11])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            g_waypointOn = true;  // turn waypoints on in case

            const int radiusValue[] = { 0, 8, 16, 32, 48, 64, 80, 96, 128 };

            if ((selection >= 1) && (selection <= 9))
               waypoints.SetRadius (radiusValue[selection - 1]);

            return true;
         }
         else if (client->menu == &g_menus[0])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            fillCommand = false;
            DisplayMenuToClient (ent, &g_menus[2]);
            break;

            case 2:
            DisplayMenuToClient (ent, &g_menus[1]);
            break;

            case 3:
            fillCommand = true;
            DisplayMenuToClient (ent, &g_menus[6]);
            break;

            case 4:
            bots.KillAll ();
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;

            }
            return true;
         }
         else if (client->menu == &g_menus[2])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            bots.AddRandom ();
            break;

            case 2:
            DisplayMenuToClient (ent, &g_menus[5]);
            break;

            case 3:
            bots.RemoveRandom ();
            break;

            case 4:
            bots.RemoveAll ();
            break;

            case 5:
            bots.RemoveMenu (ent, 1);
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[1])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            DisplayMenuToClient (ent, &g_menus[3]);
            break;

            case 2:
            DisplayMenuToClient (ent, &g_menus[9]);
            break;

            case 3:
            DisplayMenuToClient (ent, &g_menus[4]);
            break;

            case 4:
            extern ConVar yb_debug;
            yb_debug.SetInt (yb_debug.GetInt () ^ 1);

            break;

            case 5:
            if (IsAlive (ent))
               DisplayMenuToClient (ent, &g_menus[18]);
            else
            {
               DisplayMenuToClient (ent, nullptr); // reset menu display
               engine.CenterPrintf ("You're dead, and have no access to this menu");
            }
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[18])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display
            Bot *bot = nullptr;

            switch (selection)
            {
            case 1:
            case 2:
            if (FindNearestPlayer (reinterpret_cast <void **> (&bot), client->ent, 300.0f, true, true, true))
            {
               if (!bot->m_hasC4 && !bot->HasHostage ())
               {
                  if (selection == 1)
                  {
                     bot->ResetDoubleJumpState ();

                     bot->m_doubleJumpOrigin = client->ent->v.origin;
                     bot->m_doubleJumpEntity = client->ent;

                     bot->PushTask (TASK_DOUBLEJUMP, TASKPRI_DOUBLEJUMP, -1, engine.Time (), true);
                     bot->TeamSayText (FormatBuffer ("Ok %s, i will help you!", ValveString::Get (ent->v.netname)));
                  }
                  else if (selection == 2)
                     bot->ResetDoubleJumpState ();
                  break;
               }
            }
            break;

            case 3:
            case 4:
            if (FindNearestPlayer (reinterpret_cast <void **> (&bot), ent, 300.0f, true, true, true))
               bot->DiscardWeaponForUser (ent, selection == 4 ? false : true);
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[19])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            const float autoDistanceValue[] = { 0.0f, 100.0f, 130.0f, 160.0f, 190.0f, 220.0f, 250.0f };

            if (selection >= 1 && selection <= 7)
               g_autoPathDistance = autoDistanceValue[selection - 1];

            if (g_autoPathDistance == 0)
               engine.CenterPrintf ("AutoPath disabled");
            else
               engine.CenterPrintf ("AutoPath maximum distance set to %f", g_autoPathDistance);

            return true;
         }
         else if (client->menu == &g_menus[20])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            waypoints.CreatePath (CONNECTION_OUTGOING);
            break;

            case 2:
            waypoints.CreatePath (CONNECTION_INCOMING);
            break;

            case 3:
            waypoints.CreatePath (CONNECTION_BOTHWAYS);
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }

            return true;
         }
         else if (client->menu == &g_menus[5])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            client->menu = &g_menus[4];

            switch (selection)
            {
            case 1:
            g_storeAddbotVars[0] = 0;
            break;

            case 2:
            g_storeAddbotVars[0] = 1;
            break;

            case 3:
            g_storeAddbotVars[0] = 2;
            break;

            case 4:
            g_storeAddbotVars[0] = 3;
            break;

            case 5:
            g_storeAddbotVars[0] = 4;
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }

            if (client->menu == &g_menus[4])
               DisplayMenuToClient (ent, &g_menus[4]);

            return true;
         }
         else if (client->menu == &g_menus[6] && fillCommand)
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            // turn off cvars if specified team
            g_engfuncs.pfnCVarSetString ("mp_limitteams", "0");
            g_engfuncs.pfnCVarSetString ("mp_autoteambalance", "0");

            case 5:
            fillServerTeam = selection;
            DisplayMenuToClient (ent, &g_menus[5]);
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[4] && fillCommand)
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            bots.FillServer (fillServerTeam, selection - 2, g_storeAddbotVars[0]);

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[6])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 5:
            g_storeAddbotVars[1] = selection;
            if (selection == 5)
            {
               g_storeAddbotVars[2] = 5;
               bots.AddBot ("", g_storeAddbotVars[0], g_storeAddbotVars[3], g_storeAddbotVars[1], g_storeAddbotVars[2]);
            }
            else
            {
               if (selection == 1)
                  DisplayMenuToClient (ent, &g_menus[7]);
               else
                  DisplayMenuToClient (ent, &g_menus[8]);
            }
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[4])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            g_storeAddbotVars[3] = selection - 2;
            DisplayMenuToClient (ent, &g_menus[6]);
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[7] || client->menu == &g_menus[8])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            g_storeAddbotVars[2] = selection;
            bots.AddBot ("", g_storeAddbotVars[0], g_storeAddbotVars[3], g_storeAddbotVars[1], g_storeAddbotVars[2]);
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[3])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            bots.SetWeaponMode (selection);
            break;

            case 10:
            DisplayMenuToClient (ent, nullptr);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[14])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            bots.GetBot (selection - 1)->Kick ();
            break;

            case 9:
            bots.RemoveMenu (ent, 2);
            break;

            case 10:
            DisplayMenuToClient (ent, &g_menus[2]);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[15])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            bots.GetBot (selection + 8 - 1)->Kick ();
            break;

            case 9:
            bots.RemoveMenu (ent, 3);
            break;

            case 10:
            bots.RemoveMenu (ent, 1);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[16])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            bots.GetBot (selection + 16 - 1)->Kick ();
            break;

            case 9:
            bots.RemoveMenu (ent, 4);
            break;

            case 10:
            bots.RemoveMenu (ent, 2);
            break;
            }
            return true;
         }
         else if (client->menu == &g_menus[17])
         {
            DisplayMenuToClient (ent, nullptr); // reset menu display

            switch (selection)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            bots.GetBot (selection + 24 - 1)->Kick ();
            break;

            case 10:
            bots.RemoveMenu (ent, 3);
            break;
            }
            return true;
         }
      }
   }

   if (!engine.IsBotCommand () && (strcmp (command, "say") == 0 || strcmp (command, "say_team") == 0))
   {
      Bot *bot = nullptr;

      if (strcmp (arg1, "dropme") == 0 || strcmp (arg1, "dropc4") == 0)
      {
         if (FindNearestPlayer (reinterpret_cast <void **> (&bot), ent, 300.0f, true, true, true))
            bot->DiscardWeaponForUser (ent, IsNullString (strstr (arg1, "c4")) ? false : true);

         return true;
      }

      bool isAlive = IsAlive (ent);
      int team = -1;

      if (strcmp (command, "say_team") == 0)
         team = engine.GetTeam (ent);

      for (int i = 0; i < engine.MaxClients (); i++)
      {
         if (!(g_clients[i].flags & CF_USED) || (team != -1 && team != g_clients[i].team) || isAlive != IsAlive (g_clients[i].ent))
            continue;

         Bot *target = bots.GetBot (i);

         if (target != nullptr)
         {
            target->m_sayTextBuffer.entityIndex = engine.IndexOfEntity (ent);

            if (IsNullString (g_engfuncs.pfnCmd_Args ()))
               continue;

            strncpy (target->m_sayTextBuffer.sayText, g_engfuncs.pfnCmd_Args (), SIZEOF_CHAR (target->m_sayTextBuffer.sayText));
            target->m_sayTextBuffer.timeNextChat = engine.Time () + target->m_sayTextBuffer.chatDelay;
         }
      }
   }
   int clientIndex = engine.IndexOfEntity (ent) - 1;

   // check if this player alive, and issue something
   if ((g_clients[clientIndex].flags & CF_ALIVE) && g_radioSelect[clientIndex] != 0 && strncmp (command, "menuselect", 10) == 0)
   {
      int radioCommand = atoi (arg1);

      if (radioCommand != 0)
      {
         radioCommand += 10 * (g_radioSelect[clientIndex] - 1);

         if (radioCommand != Radio_Affirmative && radioCommand != Radio_Negative && radioCommand != Radio_ReportingIn)
         {
            for (int i = 0; i < engine.MaxClients (); i++)
            {
               Bot *bot = bots.GetBot (i);

               // validate bot
               if (bot != nullptr && bot->m_team == g_clients[clientIndex].team && ent != bot->GetEntity () && bot->m_radioOrder == 0)
               {
                  bot->m_radioOrder = radioCommand;
                  bot->m_radioEntity = ent;
               }
            }
         }
         g_lastRadioTime[g_clients[clientIndex].team] = engine.Time ();
      }
      g_radioSelect[clientIndex] = 0;
   }
   else if (strncmp (command, "radio", 5) == 0)
      g_radioSelect[clientIndex] = atoi (&command[5]);

   return false;
}

void GameEventsListener::OnChangeLevel (void)
{
   waypoints.SaveExperienceTab ();
   waypoints.SaveVisibilityTab ();
}
