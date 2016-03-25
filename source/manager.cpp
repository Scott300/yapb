//
// Yet Another POD-Bot, based on PODBot by Markus Klinge ("CountFloyd").
// Copyright (c) YaPB Development Team.
//
// This software is licensed under the BSD-style license.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     http://yapb.jeefo.net/license
//

#include <core.h>

ConVar yb_autovacate ("yb_autovacate", "0");
ConVar yb_autovacate_smart_kick ("yb_autovacate_smart_kick", "1");

ConVar yb_quota ("yb_quota", "0", VT_NORMAL);
ConVar yb_quota_mode ("yb_quota_mode", "normal");

ConVar yb_join_after_player ("yb_join_after_player", "0");
ConVar yb_join_team ("yb_join_team", "any");

ConVar yb_name_prefix ("yb_name_prefix", "");
ConVar yb_difficulty ("yb_difficulty", "4");

ConVar yb_latency_display ("yb_latency_display", "2");
ConVar yb_avatar_display ("yb_avatar_display", "1");

BotManager::BotManager (void)
{
   // this is a bot manager class constructor

   m_lastWinner = -1;
   m_deathMsgSent = false;

   m_economicsGood[TERRORIST] = true;
   m_economicsGood[CT] = true;

   memset (m_bots, 0, sizeof (m_bots));

   m_maintainTime = 0.0f;
   m_quotaMaintainTime = 0.0f;
   m_grenadeUpdateTime = 0.0f;

   m_creationTab.RemoveAll ();
   m_balanceCount = 0;
}

BotManager::~BotManager (void)
{
   // this is a bot manager class destructor, do not use engine.MaxClients () here !!
   Free ();
}


BotCreationResult BotManager::CreateBot (const String &name, int difficulty, int personality, int team, int member)
{
   // this function completely prepares bot entity (edict) for creation, creates team, difficulty, sets name etc, and
   // then sends result to bot constructor

   
   char outputName[33];

   if (g_numWaypoints < 1) // don't allow creating bots with no waypoints loaded
   {
      engine.CenterPrintf ("Map is not waypointed. Cannot create bot");
      return BOT_RESULT_NAV_ERROR;
   }
   else if (g_waypointsChanged) // don't allow creating bots with changed waypoints (distance tables are messed up)
   {
      engine.CenterPrintf ("Waypoints have been changed. Load waypoints again...");
      return BOT_RESULT_NAV_ERROR;
   }

   if (difficulty < 0 || difficulty > 4)
      difficulty = yb_difficulty.GetInt ();

   if (difficulty < 0 || difficulty > 4)
   {
      difficulty = Random.Long (3, 4);
      yb_difficulty.SetInt (difficulty);
   }

   if (personality < 0 || personality > 2)
   {
      if (Random.Long (0, 100) < 50)
         personality = PERSONALITY_NORMAL;
      else
      {
         if (Random.Long (0, 100) < 65)
            personality = PERSONALITY_RUSHER;
         else
            personality = PERSONALITY_CAREFUL;
      }
   }
   String steamId = "";

   // setup name
   if (name.IsEmpty ())
   {
      if (!g_botNames.IsEmpty ())
      {
         bool nameFound = false;

         for (int i = 0; i < g_botNames.GetSize (); i++)
         {
            if (nameFound)
               break;

            BotName *pickedName = &g_botNames.GetRandomElement ();

            if (pickedName == nullptr)
               continue;

            if (pickedName->used)
               continue;

            pickedName->used = nameFound = true;
            strncpy (outputName, pickedName->name, SIZEOF_CHAR (outputName));

            steamId = pickedName->steamId;
         }
      }
      else
         sprintf (outputName, "bot%i", Random.Long (0, 100)); // just pick ugly random name
   }
   else
      strncpy (outputName, name, 21);

   if (!IsNullString (yb_name_prefix.GetString ()))
   {
      char prefixedName[33]; // temp buffer for storing modified name

      if (!IsNullString (yb_name_prefix.GetString ()))
         sprintf (prefixedName, "%s %s", yb_name_prefix.GetString (), outputName);

      // buffer has been modified, copy to real name
      if (!IsNullString (prefixedName))
         strcpy (outputName, prefixedName);
   }
   edict_t *bot = nullptr;

   if (engine.IsNullEntity ((bot = gs.ConstructFakeClient (outputName, yb_latency_display.GetInt (), yb_avatar_display.GetBool (), steamId.GetBuffer ()))))
   {
      engine.CenterPrintf ("Maximum players reached (%d/%d). Unable to create Bot.", engine.MaxClients (), engine.MaxClients ());
      return BOT_RESULT_MAX_PLAYERS_REACHED;
   }
   int index = engine.IndexOfEntity (bot) - 1;

   BOT_ASSERT (index >= 0 && index <= MAX_ENGINE_PLAYERS); // check index
   BOT_ASSERT (m_bots[index] == nullptr); // check bot slot

   m_bots[index] = new Bot (bot, difficulty, personality, team, member);

   if (m_bots[index] == nullptr)
      BOT_MEMORY_FAILURE ();

   engine.Printf ("Connecting Bot...");

   // bot was successfully created 
   return BOT_RESULT_CREATED;
}

int BotManager::GetIndex (edict_t *ent)
{
   // this function returns index of bot (using own bot array)
   if (engine.IsNullEntity (ent))
      return -1;

   int index = engine.IndexOfEntity (ent) - 1;

   if (index < 0 || index >= MAX_ENGINE_PLAYERS)
      return -1;

   if (m_bots[index] != nullptr)
      return index;

   return -1; // if no edict, return -1;
}

Bot *BotManager::GetBot (int index)
{
   // this function finds a bot specified by index, and then returns pointer to it (using own bot array)

   if (index < 0 || index >= MAX_ENGINE_PLAYERS)
      return nullptr;

   return m_bots[index];
}

Bot *BotManager::GetBot (edict_t *ent)
{
   // same as above, but using bot entity

   return GetBot (GetIndex (ent));
}

Bot *BotManager::FindOneValidAliveBot (void)
{
   // this function finds one bot, alive bot :)

   Array <int> foundBots;

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (foundBots.GetSize () > 4)
         break;

      if (m_bots[i] != nullptr && IsAlive (m_bots[i]->GetEntity ()))
         foundBots.Push (i);
   }

   if (!foundBots.IsEmpty ())
      return m_bots[foundBots.GetRandomElement ()];

   return nullptr;
}

void BotManager::Think (void)
{
   // this function calls think () function for all available at call moment bots

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr)
         m_bots[i]->Think ();
   }
}

void BotManager::PeriodicThink (void)
{
   // this function calls periodic SecondThink () function for all available at call moment bots

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr)
         m_bots[i]->PeriodicThink ();
   }
}

void BotManager::AddBot (const String &name, int difficulty, int personality, int team, int member)
{
   // this function putting bot creation process to queue to prevent engine crashes

   CreateQueue bot;

   // fill the holder
   bot.name = name;
   bot.difficulty = difficulty;
   bot.personality = personality;
   bot.team = team;
   bot.member = member;

   // put to queue
   m_creationTab.Push (bot);
}

void BotManager::AddBot (const String &name, const String &difficulty, const String &personality, const String &team, const String &member)
{
   // this function is same as the function above, but accept as parameters string instead of integers

   CreateQueue bot;
   const String &any = "*";

   bot.name = (name.IsEmpty () || name == any) ? String ("\0") : name;
   bot.difficulty = (difficulty.IsEmpty () || difficulty == any) ? -1 : difficulty.ToInt ();
   bot.team = (team.IsEmpty () || team == any) ? -1 : team.ToInt ();
   bot.member = (member.IsEmpty () || member == any) ? -1 : member.ToInt ();
   bot.personality = (personality.IsEmpty () || personality == any) ? -1 : personality.ToInt ();

   m_creationTab.Push (bot);
}

void BotManager::AdjustQuota (bool isPlayerConnection, edict_t *ent)
{
   // this function increases or decreases bot quota amount depending on auto vacate variables

   if (!engine.IsDedicatedServer () || !yb_autovacate.GetBool () || GetBot (ent) != nullptr)
      return;

   if (isPlayerConnection)
   {
      if (yb_autovacate_smart_kick.GetBool ())
         AddPlayerToCheckTeamQueue (ent);
      else
      {
         RemoveRandom ();
         m_balanceCount--;
      }
   }
   else if (m_balanceCount <= 0)
   {
      AddRandom ();
      m_balanceCount++;
   }
}

void BotManager::AddPlayerToCheckTeamQueue (edict_t *ent)
{
   // entity must be unique
   bool hasFound = false;

   FOR_EACH_AE (m_trackedPlayers, it)
   {
      if (m_trackedPlayers[it] == ent)
      {
         hasFound = true;
         break;
      }
   }

   if (!hasFound)
      m_trackedPlayers.Push (ent);
}

void BotManager::VerifyPlayersHasJoinedTeam (int &desiredCount)
{
   if (!m_trackedPlayers.GetElementNumber ())
      return;

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Client &cl = g_clients[i];

      if ((cl.flags & CF_USED) && cl.team != SPECTATOR && !IsValidBot (cl.ent))
      {
         FOR_EACH_AE (m_trackedPlayers, it)
         {
            if (cl.ent != m_trackedPlayers[it])
               continue;

            m_balanceCount--;
            desiredCount--;

            m_trackedPlayers.RemoveAt (it);

            break;
         }
      }
   }
}

void BotManager::MaintainBotQuota (void)
{
   // this function keeps number of bots up to date, and don't allow to maintain bot creation
   // while creation process in process.

   if (g_numWaypoints < 1 || g_waypointsChanged)
      return;

   // bot's creation update
   if (!m_creationTab.IsEmpty () && m_maintainTime < engine.Time ())
   {
      const CreateQueue &last = m_creationTab.Pop ();
      BotCreationResult callResult = CreateBot (last.name, last.difficulty, last.personality, last.team, last.member);

      // check the result of creation
      if (callResult == BOT_RESULT_NAV_ERROR)
      {
         m_creationTab.RemoveAll (); // something wrong with waypoints, reset tab of creation
         yb_quota.SetInt (0); // reset quota
      }
      else if (callResult == BOT_RESULT_MAX_PLAYERS_REACHED)
      {
         m_creationTab.RemoveAll (); // maximum players reached, so set quota to maximum players
         yb_quota.SetInt (GetBotsNum ());
      }
      m_maintainTime = engine.Time () + 0.20f;
   }

   // now keep bot number up to date
   if (m_quotaMaintainTime < engine.Time ())
   {
      // don't allow that quota is below zero
      if (yb_quota.GetInt () < 0)
         yb_quota.SetInt (0);

      int numBots = GetBotsNum ();
      int numHumans = GetHumansJoinedTeam ();
      int desiredCount = yb_quota.GetInt ();

      if (yb_join_after_player.GetBool() && !numHumans)
         desiredCount = 0;

      // quota mode
      if (numHumans > 0)
      {
         char mode = yb_quota_mode.GetString ()[0];

         if (mode == 'f') // fill
            desiredCount = max (0, desiredCount - numHumans);
         else if (mode == 'm') // match
            desiredCount = max (0, yb_quota.GetInt () * numHumans);

         if (yb_autovacate.GetBool ())
            desiredCount = min (desiredCount, engine.MaxClients () - (numHumans + 1));
         else
            desiredCount = min (desiredCount, engine.MaxClients () - numHumans);

         if (yb_autovacate_smart_kick.GetBool () && numBots > 1 && desiredCount > 1)
            VerifyPlayersHasJoinedTeam (desiredCount);
      }

      if (desiredCount > numBots)
         AddRandom ();
      else if (desiredCount < numBots)
         RemoveRandom ();

      m_quotaMaintainTime = engine.Time () + 0.40f;
   }
}

void BotManager::InitQuota (void)
{
   m_balanceCount = 0;

   m_maintainTime = engine.Time () + 3.0f;
   m_quotaMaintainTime = engine.Time () + 3.0f;

   m_trackedPlayers.RemoveAll ();
   m_creationTab.RemoveAll ();
}

void BotManager::FillServer (int selection, int personality, int difficulty, int numToAdd)
{
   // this function fill server with bots, with specified team & personality

   if (GetBotsNum () >= engine.MaxClients () - GetHumansNum ())
      return;

   if (selection == 1 || selection == 2)
   {
      g_engfuncs.pfnCVarSetString ("mp_limitteams", "0");
      g_engfuncs.pfnCVarSetString ("mp_autoteambalance", "0");
   }
   else
      selection = 5;

   char teamDesc[6][12] =
   {
      "",
      {"Terrorists"},
      {"CTs"},
      "",
      "",
      {"Random"},
   };

   int toAdd = numToAdd == -1 ? engine.MaxClients () - (GetHumansNum () + GetBotsNum ()) : numToAdd;

   for (int i = 0; i <= toAdd; i++)
      AddBot ("", difficulty, personality, selection, -1);

   yb_quota.SetInt (toAdd);
   engine.CenterPrintf ("Fill Server with %s bots...", &teamDesc[selection][0]);
}

void BotManager::RemoveAll (bool zeroQuota)
{
   // this function drops all bot clients from server (this function removes only yapb's)`q

   if (zeroQuota)
      engine.CenterPrintf ("Bots are removed from server.");

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr)  // is this slot used?
         m_bots[i]->Kick ();
   }
   m_creationTab.RemoveAll ();

   // reset cvars
   if (zeroQuota)
   {
      yb_quota.SetInt (0);
      yb_autovacate.SetInt (0);
   }
}

void BotManager::RemoveFromTeam (Team team, bool removeAll)
{
   // this function remove random bot from specified team (if removeAll value = 1 then removes all players from team)

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr && team == engine.GetTeam (m_bots[i]->GetEntity ()))
      {
         m_bots[i]->Kick ();

         if (!removeAll)
            break;
      }
   }
}

void BotManager::RemoveMenu (edict_t *ent, int selection)
{
   // this function displays remove bot menu to specified entity (this function show's only yapb's).

   if (selection > 4 || selection < 1)
      return;

   static char tempBuffer[1024];
   char buffer[1024];

   memset (tempBuffer, 0, sizeof (tempBuffer));
   memset (buffer, 0, sizeof (buffer));

   int validSlots = (selection == 4) ? (1 << 9) : ((1 << 8) | (1 << 9));

   for (int i = ((selection - 1) * 8); i < selection * 8; i++)
   {
      if ((m_bots[i] != nullptr) && !engine.IsNullEntity (m_bots[i]->GetEntity ()))
      {
         validSlots |= 1 << (i - ((selection - 1) * 8));
         sprintf (buffer, "%s %1.1d. %s%s\n", buffer, i - ((selection - 1) * 8) + 1, ValveString::Get (m_bots[i]->pev->netname), engine.GetTeam (m_bots[i]->GetEntity ()) == CT ? " \\y(CT)\\w" : " \\r(T)\\w");
      }
      else
         sprintf (buffer, "%s\\d %1.1d. Not a Bot\\w\n", buffer, i - ((selection - 1) * 8) + 1);
   }

   sprintf (tempBuffer, "\\yBots Remove Menu (%d/4):\\w\n\n%s\n%s 0. Back", selection, buffer, (selection == 4) ? "" : " 9. More...\n");

   switch (selection)
   {
   case 1:
      g_menus[14].validSlots = validSlots & static_cast <unsigned int> (-1);
      g_menus[14].menuText = tempBuffer;

      DisplayMenuToClient (ent, &g_menus[14]);
      break;

   case 2:
      g_menus[15].validSlots = validSlots & static_cast <unsigned int> (-1);
      g_menus[15].menuText = tempBuffer;

      DisplayMenuToClient (ent, &g_menus[15]);
      break;

    case 3:
      g_menus[16].validSlots = validSlots & static_cast <unsigned int> (-1);
      g_menus[16].menuText = tempBuffer;

      DisplayMenuToClient (ent, &g_menus[16]);
      break;

   case 4:
      g_menus[17].validSlots = validSlots & static_cast <unsigned int> (-1);
      g_menus[17].menuText = tempBuffer;

      DisplayMenuToClient (ent, &g_menus[17]);
      break;
   }
}

void BotManager::KillAll (int team)
{
   // this function kills all bots on server (only this dll controlled bots)

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr)
      {
         if (team != -1 && team != m_bots[i]->m_team)
            continue;

         m_bots[i]->Kill ();
      }
   }
   engine.CenterPrintf ("All Bots died !");
}

void BotManager::RemoveRandom (void)
{
   // this function removes random bot from server

   // first try to kick the bot that is currently dead
   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr && !m_bots[i]->m_notKilled)  // is this slot used?
      {
         m_bots[i]->Kick ();
         return;
      }
   }

   // if no dead bots found try to find one with lowest amount of frags
   int index = 0;
   float score = 9999.0f;

   // search bots in this team
   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Bot *bot = bots.GetBot (i);

      if (bot != nullptr && bot->pev->frags < score)
      {
         index = i;
         score = bot->pev->frags;
      }
   }

   // if found some bots
   if (index != 0)
   {
      m_bots[index]->Kick ();
      return;
   }

   // worst case, just kick some random bot
   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr)  // is this slot used?
      {
         m_bots[i]->Kick ();
         break;
      }
   }
}

void BotManager::SetWeaponMode (int selection)
{
   // this function sets bots weapon mode

   int tabMapStandart[7][NUM_WEAPONS] =
   {
      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Knife only
      {-1,-1,-1, 2, 2, 0, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Pistols only
      {-1,-1,-1,-1,-1,-1,-1, 2, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Shotgun only
      {-1,-1,-1,-1,-1,-1,-1,-1,-1, 2, 1, 2, 0, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 2,-1}, // Machine Guns only
      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 1, 0, 1, 1,-1,-1,-1,-1,-1,-1}, // Rifles only
      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 2, 2, 0, 1,-1,-1}, // Snipers only
      {-1,-1,-1, 2, 2, 0, 1, 2, 2, 2, 1, 2, 0, 2, 0, 0, 1, 0, 1, 1, 2, 2, 0, 1, 2, 1}  // Standard
   };

   int tabMapAS[7][NUM_WEAPONS] =
   {
      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Knife only
      {-1,-1,-1, 2, 2, 0, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Pistols only
      {-1,-1,-1,-1,-1,-1,-1, 1, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Shotgun only
      {-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1, 1, 0, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1}, // Machine Guns only
      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1, 1, 0, 1, 1,-1,-1,-1,-1,-1,-1}, // Rifles only
      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0,-1, 1,-1,-1}, // Snipers only
      {-1,-1,-1, 2, 2, 0, 1, 1, 1, 1, 1, 1, 0, 2, 0,-1, 1, 0, 1, 1, 0, 0,-1, 1, 1, 1}  // Standard
   };

   char modeName[7][12] =
   {
      {"Knife"},
      {"Pistol"},
      {"Shotgun"},
      {"Machine Gun"},
      {"Rifle"},
      {"Sniper"},
      {"Standard"}
   };
   selection--;

   for (int i = 0; i < NUM_WEAPONS; i++)
   {
      g_weaponSelect[i].teamStandard = tabMapStandart[selection][i];
      g_weaponSelect[i].teamAS = tabMapAS[selection][i];
   }

   if (selection == 0)
      yb_jasonmode.SetInt (1);
   else
      yb_jasonmode.SetInt (0);

   engine.CenterPrintf ("%s weapon mode selected", &modeName[selection][0]);
}

void BotManager::ListBots (void)
{
   // this function list's bots currently playing on the server

   engine.Printf ("%-3.5s %-9.13s %-17.18s %-3.4s %-3.4s %-3.4s", "index", "name", "personality", "team", "difficulty", "frags");

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Bot *bot = GetBot (i);

      // is this player slot valid
      if (bot != nullptr)
         engine.Printf ("[%-3.1d] %-9.13s %-17.18s %-3.4s %-3.1d %-3.1d", i, ValveString::Get (bot->pev->netname), bot->m_personality == PERSONALITY_RUSHER ? "rusher" : bot->m_personality == PERSONALITY_NORMAL ? "normal" : "careful", bot->m_team == CT ? "CT" : "T", bot->m_difficulty, static_cast <int> (bot->pev->frags));
   }
}

int BotManager::GetBotsNum (void)
{
   // this function returns number of yapb's playing on the server

   int count = 0;

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr)
         count++;
   }
   return count;
}

Bot *BotManager::GetHighestFragsBot (int team)
{
   int bestIndex = 0;
   float bestScore = -1;

   // search bots in this team
   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Bot *bot = bots.GetBot (i);

      if (bot != nullptr && bot->m_notKilled && bot->m_team == team)
      {
         if (bot->pev->frags > bestScore)
         {
            bestIndex = i;
            bestScore = bot->pev->frags;
         }
      }
   }
   return GetBot (bestIndex);
}

void BotManager::CheckTeamEconomics (int team, bool setTrue)
{
   // this function decides is players on specified team is able to buy primary weapons by calculating players
   // that have not enough money to buy primary (with economics), and if this result higher 80%, player is can't
   // buy primary weapons.

   extern ConVar yb_economics_rounds;

   if (setTrue || !yb_economics_rounds.GetBool ())
   {
      m_economicsGood[team] = true;
      return; // don't check economics while economics disable
   }

   int numPoorPlayers = 0;
   int numTeamPlayers = 0;

   // start calculating
   for (int i = 0; i < engine.MaxClients (); i++)
   {
      if (m_bots[i] != nullptr && engine.GetTeam (m_bots[i]->GetEntity ()) == team)
      {
         if (m_bots[i]->m_moneyAmount <= g_botBuyEconomyTable[0])
            numPoorPlayers++;

         numTeamPlayers++; // update count of team
      }
   }
   m_economicsGood[team] = true;

   if (numTeamPlayers <= 1)
      return;

   // if 80 percent of team have no enough money to purchase primary weapon
   if ((numTeamPlayers * 80) / 100 <= numPoorPlayers)
      m_economicsGood[team] = false;

   // winner must buy something!
   if (m_lastWinner == team)
      m_economicsGood[team] = true;
}

void BotManager::Free (void)
{
   // this function free all bots slots (used on server shutdown)

   for (int i = 0; i < MAX_ENGINE_PLAYERS; i++)
      Free (i);
}

void BotManager::Free (int index)
{
   // this function frees one bot selected by index (used on bot disconnect)

   delete m_bots[index];
   m_bots[index] = nullptr;
}

Bot::Bot (edict_t *bot, int difficulty, int personality, int team, int member)
{
   // this function does core operation of creating bot, it's called by CreateBot (),
   // when bot setup completed, (this is a bot class constructor)

   // reset stuff
   memset (&m_pingOffset, 0, sizeof (m_pingOffset));
   memset (&m_ping, 0, sizeof (m_ping));
   memset (reinterpret_cast <void *> (this), 0, sizeof (*this));

   pev = &bot->v;

   // initialize all the variables for this bot...
   m_notStarted = true;  // hasn't joined game yet

   m_startAction = GSM_IDLE;
   m_moneyAmount = 0;
   m_logotypeIndex = Random.Long (0, 9);

   // assign how talkative this bot will be
   m_sayTextBuffer.chatDelay = Random.Float (3.8f, 10.0f);
   m_sayTextBuffer.chatProbability = Random.Long (1, 100);

   m_notKilled = false;
   m_weaponBurstMode = BM_OFF;
   m_difficulty = difficulty;

   if (difficulty < 0 || difficulty > 4)
   {
      difficulty = Random.Long (3, 4);
      yb_difficulty.SetInt (difficulty);
   }

   m_lastCommandTime = engine.Time () - 0.1f;
   m_frameInterval = engine.Time ();
   m_timePeriodicUpdate = 0.0f;

   switch (personality)
   {
   case 1:
      m_personality = PERSONALITY_RUSHER;
      m_baseAgressionLevel = Random.Float (0.7f, 1.0f);
      m_baseFearLevel = Random.Float (0.0f, 0.4f);
      break;

   case 2:
      m_personality = PERSONALITY_CAREFUL;
      m_baseAgressionLevel = Random.Float (0.2f, 0.5f);
      m_baseFearLevel = Random.Float (0.7f, 1.0f);
      break;

   default:
      m_personality = PERSONALITY_NORMAL;
      m_baseAgressionLevel = Random.Float (0.4f, 0.7f);
      m_baseFearLevel = Random.Float (0.4f, 0.7f);
      break;
   }

   memset (&m_ammoInClip, 0, sizeof (m_ammoInClip));
   memset (&m_ammo, 0, sizeof (m_ammo));

   m_currentWeapon = 0; // current weapon is not assigned at start
   m_voicePitch = Random.Long (80, 115); // assign voice pitch

   // copy them over to the temp level variables
   m_agressionLevel = m_baseAgressionLevel;
   m_fearLevel = m_baseFearLevel;
   m_nextEmotionUpdate = engine.Time () + 0.5f;

   // just to be sure
   m_actMessageIndex = 0;
   m_pushMessageIndex = 0;

   // assign team and class
   m_wantedTeam = team;
   m_wantedClass = member;

   int newBotsNum = bots.GetBotsNum () + 1;

   // keep quota number up to date
   if (newBotsNum < engine.MaxClients () && newBotsNum > yb_quota.GetInt ())
      yb_quota.SetInt (newBotsNum);

   NewRound ();
}

void Bot::ReleaseUsedName (void)
{
   FOR_EACH_AE (g_botNames, j)
   {
      BotName &name = g_botNames[j];

      if (strcmp (name.name, ValveString::Get (pev->netname)) == 0)
      {
         name.used = false;
         break;
      }
   }
}

Bot::~Bot (void)
{
   // this is bot destructor

   DeleteSearchNodes ();
   ResetTasks ();
}

int BotManager::GetHumansNum (void)
{
   // this function returns number of humans playing on the server

   int count = 0;

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Client *cl = &g_clients[i];

      if ((cl->flags & CF_USED) && m_bots[i] == nullptr && !(cl->ent->v.flags & FL_FAKECLIENT))
         count++;
   }
   return count;
}

int BotManager::GetHumansAliveNum (void)
{
   // this function returns number of humans playing on the server

   int count = 0;

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Client *cl = &g_clients[i];

      if ((cl->flags & (CF_USED | CF_ALIVE)) && m_bots[i] == nullptr && !(cl->ent->v.flags & FL_FAKECLIENT))
         count++;
   }
   return count;
}

int BotManager::GetHumansJoinedTeam (void)
{
   // this function returns number of humans playing on the server

   int count = 0;

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Client *cl = &g_clients[i];

      if ((cl->flags & (CF_USED | CF_ALIVE)) && m_bots[i] == nullptr && cl->team != SPECTATOR && !(cl->ent->v.flags & FL_FAKECLIENT) && cl->ent->v.movetype != MOVETYPE_FLY)
         count++;
   }
   return count;
}

void Bot::NewRound (void)
{     
   // this function initializes a bot after creation & at the start of each round
   
   g_canSayBombPlanted = true;
   int i = 0;

   // delete all allocated path nodes
   DeleteSearchNodes ();

   m_waypointOrigin.Zero ();
   m_destOrigin.Zero ();
   m_currentWaypointIndex = -1;
   m_currentPath = nullptr;
   m_currentTravelFlags = 0;
   m_goalFailed = 0;
   m_desiredVelocity.Zero ();
   m_prevGoalIndex = -1;
   m_chosenGoalIndex = -1;
   m_loosedBombWptIndex = -1;

   m_moveToC4 = false;
   m_duckDefuse = false;
   m_duckDefuseCheckTime = 0.0f;

   m_numFriendsLeft = 0;
   m_numEnemiesLeft = 0;

   for (i = 0; i < 5; i++)
      m_prevWptIndex[i] = -1;

   m_navTimeset = engine.Time ();
   m_team = engine.GetTeam (GetEntity ());

   switch (m_personality)
   {
   case PERSONALITY_NORMAL:
      m_pathType = Random.Long (0, 100) > 50 ? SEARCH_PATH_SAFEST_FASTER : SEARCH_PATH_SAFEST;
      break;

   case PERSONALITY_RUSHER:
      m_pathType = SEARCH_PATH_FASTEST;
      break;

   case PERSONALITY_CAREFUL:
      m_pathType = SEARCH_PATH_SAFEST;
      break;
   }

   // clear all states & tasks
   m_states = 0;
   ResetTasks ();

   m_isVIP = false;
   m_isLeader = false;
   m_hasProgressBar = false;
   m_canChooseAimDirection = true;
   m_turnAwayFromFlashbang = 0.0f;

   m_maxThrowTimer = 0.0f;
   m_timeTeamOrder = 0.0f;
   m_timeRepotingInDelay = Random.Float (40.0f, 240.0f);
   m_askCheckTime = 0.0f;
   m_minSpeed = 260.0f;
   m_prevSpeed = 0.0f;
   m_prevOrigin = Vector (9999.0f, 9999.0f, 9999.0f);
   m_prevTime = engine.Time ();
   m_blindRecognizeTime = engine.Time ();
   m_lookUpdateTime = engine.Time ();
   
   m_viewDistance = 4096.0f;
   m_maxViewDistance = 4096.0f;

   m_liftEntity = nullptr;
   m_pickupItem = nullptr;
   m_itemIgnore = nullptr;
   m_itemCheckTime = 0.0f;

   m_breakableEntity = nullptr;
   m_breakableOrigin.Zero ();
   m_timeDoorOpen = 0.0f;

   ResetCollideState ();
   ResetDoubleJumpState ();

   m_enemy = nullptr;
   m_lastVictim = nullptr;
   m_lastEnemy = nullptr;
   m_lastEnemyOrigin.Zero ();
   m_trackingEdict = nullptr;
   m_timeNextTracking = 0.0f;

   m_buttonPushTime = 0.0f;
   m_enemyUpdateTime = 0.0f;
   m_enemyIgnoreTimer = 0.0f;
   m_seeEnemyTime = 0.0f;
   m_shootAtDeadTime = 0.0f;
   m_oldCombatDesire = 0.0f;
   m_liftUsageTime = 0.0f;

   m_avoidGrenade = nullptr;
   m_needAvoidGrenade = 0;

   m_lastDamageType = -1;
   m_voteKickIndex = 0;
   m_lastVoteKick = 0;
   m_voteMap = 0;
   m_doorOpenAttempt = 0;
   m_aimFlags = 0;
   m_liftState = 0;

   m_position.Zero ();
   m_liftTravelPos.Zero ();

   SetIdealReactionTimes (true);

   m_targetEntity = nullptr;
   m_tasks.RemoveAll ();
   m_followWaitTime = 0.0f;

   for (i = 0; i < MAX_HOSTAGES; i++)
      m_hostages[i] = nullptr;

   for (i = 0; i < Chatter_Total; i++)
      m_chatterTimes[i] = -1.0f;

   m_isReloading = false;
   m_reloadState = RELOAD_NONE;

   m_reloadCheckTime = 0.0f;
   m_shootTime = engine.Time ();
   m_playerTargetTime = engine.Time ();
   m_firePause = 0.0f;
   m_timeLastFired = 0.0f;

   m_grenadeCheckTime = 0.0f;
   m_isUsingGrenade = false;

   m_blindButton = 0;
   m_blindTime = 0.0f;
   m_jumpTime = 0.0f;
   m_jumpStateTimer = 0.0f;
   m_jumpFinished = false;
   m_isStuck = false;

   m_sayTextBuffer.timeNextChat = engine.Time ();
   m_sayTextBuffer.entityIndex = -1;
   m_sayTextBuffer.sayText[0] = 0x0;

   m_buyState = BUYSTATE_PRIMARY_WEAPON;
   m_lastEquipTime = 0.0f;

   if (!m_notKilled) // if bot died, clear all weapon stuff and force buying again
   {
      memset (&m_ammoInClip, 0, sizeof (m_ammoInClip));
      memset (&m_ammo, 0, sizeof (m_ammo));

      m_currentWeapon = 0;
   }

   m_knifeAttackTime = engine.Time () + Random.Float (1.3f, 2.6f);
   m_nextBuyTime = engine.Time () + Random.Float (0.6f, 2.0f);

   m_buyPending = false;
   m_inBombZone = false;
   m_hasC4 = false;

   m_shieldCheckTime = 0.0f;
   m_zoomCheckTime = 0.0f;
   m_strafeSetTime = 0.0f;
   m_combatStrafeDir = STRAFE_DIR_NONE;
   m_fightStyle = FIGHT_NONE;
   m_lastFightStyleCheck = 0.0f;

   m_checkWeaponSwitch = true;
   m_checkKnifeSwitch = true;
   m_buyingFinished = false;

   m_radioEntity = nullptr;
   m_radioOrder = 0;
   m_defendedBomb = false;
   m_defendHostage = false;
   m_headedTime = 0.0f;

   m_timeLogoSpray = engine.Time () + Random.Float (0.5f, 2.0f);
   m_spawnTime = engine.Time ();
   m_lastChatTime = engine.Time ();

   m_timeCamping = 0;
   m_campDirection = 0;
   m_nextCampDirTime = 0;
   m_campButtons = 0;

   m_soundUpdateTime = 0.0f;
   m_heardSoundTime = engine.Time ();

   // clear its message queue
   for (i = 0; i < 32; i++)
      m_messageQueue[i] = GSM_IDLE;

   m_actMessageIndex = 0;
   m_pushMessageIndex = 0;

   // and put buying into its message queue
   PushMessageQueue (GSM_BUY_STUFF);
   PushTask (TASK_NORMAL, TASKPRI_NORMAL, -1, 0.0f, true);

   if (Random.Long (0, 100) < 50)
      ChatterMessage (Chatter_NewRound);

   const float refreshInetrval = (1.0f / 30.0f) * Random.Float (0.95f, 1.05f);
   m_thinkInterval = (g_gameFlags & GAME_LEGACY) ? 0.0f : refreshInetrval;
}

void Bot::Kill (void)
{
   // this function kills a bot (not just using ClientKill, but like the CSBot does)
   // base code courtesy of Lazy (from bots-united forums!)

   gs.SlayPlayer (GetEntity (), g_weaponDefs[m_currentWeapon].className);
}

void Bot::Kick (void)
{
   // this function kick off one bot from the server.
   
   engine.IssueCmd ("kick \"%s\"", ValveString::Get (pev->netname));
   engine.CenterPrintf ("Bot '%s' kicked", ValveString::Get (pev->netname));

   int newBotsNum = bots.GetBotsNum () - 1;

   // keep quota number up to date
   if (newBotsNum < engine.MaxClients () && newBotsNum < yb_quota.GetInt ())
      yb_quota.SetInt (newBotsNum);
}

void Bot::StartGame (void)
{
   // this function handles the selection of teams & class

#ifdef XASH_CSDM
   m_wantedTeam = Random.Long (1, 2);
   
   engine.IssueBotCommand (GetEntity (), "jointeam %d", m_wantedTeam);
   
   SET_CLIENT_KEYVALUE (GetIndex (), GET_INFOKEYBUFFER (GetEntity ()), "model", m_wantedTeam == 2 ? "Counter-Terrorists" : "Terrorists");

   if (Random.Long (0, 100) < 20)
      ChatMessage (CHAT_WELCOME);
   
   m_notStarted = false;
   return;
#endif

   // handle counter-strike stuff here...
   if (m_startAction == GSM_TEAM_SELECT)
   {
      m_startAction = GSM_IDLE;  // switch back to idle
      
      char teamJoin = yb_join_team.GetString ()[0];

      if (teamJoin == 'C' || teamJoin == 'c')
         m_wantedTeam = 2;
      else if (teamJoin == 'T' || teamJoin == 't')
         m_wantedTeam = 1;

      if (m_wantedTeam != 1 && m_wantedTeam != 2)
         m_wantedTeam = 5;

      // select the team the bot wishes to join...
      engine.IssueBotCommand (GetEntity (), "menuselect %d", m_wantedTeam);
   }
   else if (m_startAction == GSM_CLASS_SELECT)
   {
      m_startAction = GSM_IDLE;  // switch back to idle

      if (g_gameFlags & GAME_CZERO) // czero has spetsnaz and militia skins
      {
         if (m_wantedClass < 1 || m_wantedClass > 5)
            m_wantedClass = Random.Long (1, 5); //  use random if invalid
      }
      else
      {
         if (m_wantedClass < 1 || m_wantedClass > 4)
            m_wantedClass = Random.Long (1, 4); // use random if invalid
      }

      // select the class the bot wishes to use...
      engine.IssueBotCommand (GetEntity (), "menuselect %d", m_wantedClass);

      // bot has now joined the game (doesn't need to be started)
      m_notStarted = false;

      // check for greeting other players, since we connected
      if (Random.Long (0, 100) < 20)
         ChatMessage (CHAT_WELCOME);
   }
}

void BotManager::CalculatePingOffsets (void)
{
   if ((g_gameFlags & GAME_LEGACY) || yb_latency_display.GetInt () != 2)
      return;

   int averagePing = 0;
   int numHumans = 0;

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      edict_t *ent = engine.EntityOfIndex (i + 1);

      if (!IsValidPlayer (ent))
         continue;

      numHumans++;

      int ping, loss;
      g_engfuncs.pfnGetPlayerStats (ent, &ping, &loss);

      if (ping < 0 || ping > 100)
         ping = Random.Long (3, 15);

      averagePing += ping;
   }

   if (numHumans > 0)
      averagePing /= numHumans;
   else
      averagePing = Random.Long (30, 40);

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Bot *bot = GetBot (i);

      if (bot == nullptr)
         continue;

      int avgCalc = static_cast <int> (averagePing * 0.2f);
      int botPing = Random.Long (averagePing - avgCalc, averagePing + avgCalc) + Random.Long (bot->m_difficulty + 3, bot->m_difficulty + 6) + 10;

      if (botPing <= 5)
         botPing = Random.Long (10, 23);
      else if (botPing > 100)
         botPing = Random.Long (30, 40);

      for (int j = 0; j < 2; j++)
      {
         for (bot->m_pingOffset[j] = 0; bot->m_pingOffset[j] < 4; bot->m_pingOffset[j]++)
         {
            if ((botPing - bot->m_pingOffset[j]) % 4 == 0)
            {
               bot->m_ping[j] = (botPing - bot->m_pingOffset[j]) / 4;
               break;
            }
         }
      }
      bot->m_ping[2] = botPing;
   }
}

void BotManager::SendPingDataOffsets (edict_t *to)
{
   if ((g_gameFlags & GAME_LEGACY) || yb_latency_display.GetInt () != 2 || engine.IsNullEntity (to) || (to->v.flags & FL_FAKECLIENT))
      return;

   if (!(to->v.flags & FL_CLIENT) && !(((to->v.button & IN_SCORE) || !(to->v.oldbuttons & IN_SCORE))))
      return;

   static int sending;
   sending = 0;

   // message writer
   MessageWriter msg;

   for (int i = 0; i < engine.MaxClients (); i++)
   {
      Bot *bot = m_bots[i];

      if (bot == nullptr)
         continue;

      switch (sending)
      {
      case 0:
         {
            // start a new message
            msg.Begin (MSG_ONE_UNRELIABLE, SVC_SENDPINGS, nullptr, to);
            msg.Byte ((bot->m_pingOffset[sending] * 64) + (1 + 2 * i));
            msg.Short (bot->m_ping[sending]);

            sending++;
         }
      case 1:
         {
            // append additional data
            msg.Byte ((bot->m_pingOffset[sending] * 128) + (2 + 4 * i));
            msg.Short (bot->m_ping[sending]);

            sending++;
         }
      case 2:
         {
            // append additional data and end message
            msg.Byte (4 + 8 * i);
            msg.Short (bot->m_ping[sending]);
            msg.Byte (0);
            msg.End ();

            sending = 0;
         }
      }
   }

   // end message if not yet sent
   if (sending)
   {
      msg.Byte (0);
      msg.End ();
   }
}

void BotManager::SendDeathMsgFix (void)
{
   if (yb_latency_display.GetInt () == 2 && m_deathMsgSent)
   {
      m_deathMsgSent = false;

      for (int i = 0; i < engine.MaxClients (); i++)
         SendPingDataOffsets (g_clients[i].ent);
   }
}

void BotManager::UpdateActiveGrenades (void)
{
   if (m_grenadeUpdateTime > engine.Time ())
      return;

   // clear previously stored grenades
   m_activeGrenades.RemoveAll ();

   // search the map for any type of grenade
   EntityIndexer entities (ENTITY_SEARCH_CLASSNAME, "grenade");

   while (entities.FindNext ())
   {
      edict_t *grenade = entities.GetCurrentEntity ();

      // do not count c4 as a grenade
      if (strcmp (ValveString::Get (grenade->v.model) + 9, "c4.mdl") == 0)
         continue;

      m_activeGrenades.Push (grenade);
   }
   m_grenadeUpdateTime = 0.213f + engine.Time ();
}

const Array <edict_t *> &BotManager::GetActiveGrenades (void)
{
   return m_activeGrenades;
}
