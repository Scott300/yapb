/***
*
*   Copyright (c) 1999-2005, Valve Corporation. All rights reserved.
*
*   This product contains software technology licensed from Id
*   Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*   All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/

#ifndef EXTDLL_H
#define EXTDLL_H

typedef int sdk_string_t; 

enum SDK_Constants
{
   FL_FLY = (1 << 0),
   FL_SWIM = (1 << 1),
   FL_CONVEYOR = (1 << 2),
   FL_CLIENT = (1 << 3),
   FL_INWATER = (1 << 4),
   FL_MONSTER = (1 << 5),
   FL_GODMODE = (1 << 6),
   FL_NOTARGET = (1 << 7),
   FL_SKIPLOCALHOST = (1 << 8),
   FL_ONGROUND = (1 << 9),
   FL_PARTIALGROUND = (1 << 10),
   FL_WATERJUMP = (1 << 11),
   FL_FROZEN = (1 << 12),
   FL_FAKECLIENT = (1 << 13),
   FL_DUCKING = (1 << 14),
   FL_FLOAT = (1 << 15),
   FL_GRAPHED = (1 << 16),
   FL_IMMUNE_WATER = (1 << 17),
   FL_IMMUNE_SLIME = (1 << 18),
   FL_IMMUNE_LAVA = (1 << 19),
   FL_PROXY = (1 << 20),
   FL_ALWAYSTHINK = (1 << 21),
   FL_BASEVELOCITY = (1 << 22),
   FL_MONSTERCLIP = (1 << 23),
   FL_ONTRAIN = (1 << 24),
   FL_WORLDBRUSH = (1 << 25),
   FL_SPECTATOR = (1 << 26),
   FL_CUSTOMENTITY = (1 << 29),
   FL_KILLME = (1 << 30),
   FL_DORMANT = (1 << 31),

   WALKMOVE_NORMAL = 0,
   WALKMOVE_WORLDONLY,
   WALKMOVE_CHECKONLY,

   MOVETYPE_NONE = 0,
   MOVETYPE_WALK = 3,
   MOVETYPE_STEP = 4,
   MOVETYPE_FLY = 5,
   MOVETYPE_TOSS = 6,
   MOVETYPE_PUSH = 7,
   MOVETYPE_NOCLIP = 8,
   MOVETYPE_FLYMISSILE = 9,
   MOVETYPE_BOUNCE = 10,
   MOVETYPE_BOUNCEMISSILE = 11,
   MOVETYPE_FOLLOW = 12,
   MOVETYPE_PUSHSTEP = 13,

   SOLID_NOT = 0,
   SOLID_TRIGGER = 1,
   SOLID_BBOX = 2,
   SOLID_SLIDEBOX = 3,
   SOLID_BSP = 4,

   DEAD_NO = 0,
   DEAD_DYING = 1,
   DEAD_DEAD = 2,
   DEAD_RESPAWNABLE = 3,
   DEAD_DISCARDBODY = 4,

   DAMAGE_NO = 0,
   DAMAGE_YES = 1,
   DAMAGE_AIM = 2,

   EF_BRIGHTFIELD = 1,
   EF_MUZZLEFLASH = 2,
   EF_BRIGHTLIGHT = 4,
   EF_DIMLIGHT = 8,
   EF_INVLIGHT = 16,
   EF_NOINTERP = 32,
   EF_LIGHT = 64,
   EF_NODRAW = 128,

   TE_BEAMPOINTS = 0,
   TE_BEAMENTPOINT = 1,
   TE_GUNSHOT = 2,
   TE_EXPLOSION = 3,
   TE_TAREXPLOSION = 4,
   TE_SMOKE = 5,
   TE_TRACER = 6,
   TE_LIGHTNING = 7,
   TE_BEAMENTS = 8,
   TE_SPARKS = 9,
   TE_LAVASPLASH = 10,
   TE_TELEPORT = 11,
   TE_EXPLOSION2 = 12,
   TE_BSPDECAL = 13,
   TE_IMPLOSION = 14,
   TE_SPRITETRAIL = 15,
   TE_BEAM = 16,
   TE_SPRITE = 17,
   TE_BEAMSPRITE = 18,
   TE_BEAMTORUS = 19,
   TE_BEAMDISK = 20,
   TE_BEAMCYLINDER = 21,
   TE_BEAMFOLLOW = 22,
   TE_GLOWSPRITE = 23,
   TE_BEAMRING = 24,
   TE_STREAK_SPLASH = 25,
   TE_BEAMHOSE = 26,
   TE_DLIGHT = 27,
   TE_ELIGHT = 28,
   TE_TEXTMESSAGE = 29,
   TE_LINE = 30,
   TE_BOX = 31,
   TE_KILLBEAM = 99,
   TE_LARGEFUNNEL = 100,
   TE_BLOODSTREAM = 101,
   TE_SHOWLINE = 102,
   TE_BLOOD = 103,
   TE_DECAL = 104,
   TE_FIZZ = 105,
   TE_MODEL = 106,
   TE_EXPLODEMODEL = 107,
   TE_BREAKMODEL = 108,
   TE_GUNSHOTDECAL = 109,
   TE_SPRITE_SPRAY = 110,
   TE_ARMOR_RICOCHET = 111,
   TE_PLAYERDECAL = 112,
   TE_BUBBLES = 113,
   TE_BUBBLETRAIL = 114,
   TE_BLOODSPRITE = 115,
   TE_WORLDDECAL = 116,
   TE_WORLDDECALHIGH = 117,
   TE_DECALHIGH = 118,
   TE_PROJECTILE = 119,
   TE_SPRAY = 120,
   TE_PLAYERSPRITES = 121,
   TE_PARTICLEBURST = 122,
   TE_FIREFIELD = 123,
   TE_PLAYERATTACHMENT = 124,
   TE_KILLPLAYERATTACHMENTS = 125,
   TE_MULTIGUNSHOT = 126,
   TE_USERTRACER = 127,

   TE_EXPLFLAG_NONE = 0,
   TE_EXPLFLAG_NOADDITIVE = 1,
   TE_EXPLFLAG_NODLIGHTS = 2,
   TE_EXPLFLAG_NOSOUND = 4,
   TE_EXPLFLAG_NOPARTICLES = 8,

   TE_FIRE_FLAG_ALLFLOAT = 1,
   TE_FIRE_FLAG_SOMEFLOAT = 2,
   TE_FIRE_FLAG_LOOP = 4,
   TE_FIRE_FLAG_ALPHA = 8,
   TE_FIRE_FLAG_PLANAR = 16,

   TE_BOUNCE_nullptr = 0,
   TE_BOUNCE_SHELL = 1,
   TE_BOUNCE_SHOTSHELL = 2,

   MSG_BROADCAST = 0,
   MSG_ONE = 1,
   MSG_ALL = 2,
   MSG_INIT = 3,
   MSG_PVS = 4,
   MSG_PAS = 5,
   MSG_PVS_R = 6,
   MSG_PAS_R = 7,
   MSG_ONE_UNRELIABLE = 8,
   MSG_SPEC = 9,

   CONTENTS_EMPTY = -1,
   CONTENTS_SOLID = -2,
   CONTENTS_WATER = -3,
   CONTENTS_SLIME = -4,
   CONTENTS_LAVA = -5,
   CONTENTS_SKY = -6,
   CONTENTS_LADDER = -16,
   CONTENTS_FLYFIELD = -17,
   CONTENTS_GRAVITY_FLYFIELD = -18,
   CONTENTS_FOG = -19,

   CHAN_AUTO = 0,
   CHAN_WEAPON = 1,
   CHAN_VOICE = 2,
   CHAN_ITEM = 3,
   CHAN_BODY = 4,
   CHAN_STREAM = 5,
   CHAN_STATIC = 6,
   CHAN_NETWORKVOICE_BASE = 7,
   CHAN_NETWORKVOICE_END = 500,

   PITCH_NORM = 100,
   PITCH_LOW = 95,
   PITCH_HIGH = 120,

   SF_TRAIN_WAIT_RETRIGGER = 1,
   SF_TRAIN_START_ON = 4,
   SF_TRAIN_PASSABLE = 8,

   SF_TRIGGER_ALLOWMONSTERS = 1,
   SF_TRIGGER_NOCLIENTS = 2,
   SF_TRIGGER_PUSHABLES = 4,

   SF_BREAK_TRIGGER_ONLY = 1,
   SF_BREAK_TOUCH = 2,
   SF_BREAK_PRESSURE = 4,
   SF_BREAK_CROWBAR = 256,

   SF_PUSH_BREAKABLE = 128,
   SF_LIGHT_START_OFF = 1,

   SF_BRUSH_ROTATE_Y_AXIS = 0,
   SF_BRUSH_ROTATE_INSTANT = 1,
   SF_BRUSH_ROTATE_BACKWARDS = 2,
   SF_BRUSH_ROTATE_Z_AXIS = 4,
   SF_BRUSH_ROTATE_X_AXIS = 8,
   SF_PENDULUM_AUTO_RETURN = 16,
   SF_PENDULUM_PASSABLE = 32,

   SF_BRUSH_ROTATE_SMALLRADIUS = 128,
   SF_BRUSH_ROTATE_MEDIUMRADIUS = 256,
   SF_BRUSH_ROTATE_LARGERADIUS = 512,

   IN_ATTACK = (1 << 0),
   IN_JUMP = (1 << 1),
   IN_DUCK = (1 << 2),
   IN_FORWARD = (1 << 3),
   IN_BACK = (1 << 4),
   IN_USE = (1 << 5),
   IN_CANCEL = (1 << 6),
   IN_LEFT = (1 << 7),
   IN_RIGHT = (1 << 8),
   IN_MOVELEFT = (1 << 9),
   IN_MOVERIGHT = (1 << 10),
   IN_ATTACK2 = (1 << 11),
   IN_RUN = (1 << 12),
   IN_RELOAD = (1 << 13),
   IN_ALT1 = (1 << 14),
   IN_SCORE = (1 << 15),

   BREAK_TYPEMASK = 0x4F,
   BREAK_GLASS = 0x01,
   BREAK_METAL = 0x02,
   BREAK_FLESH = 0x04,
   BREAK_WOOD = 0x08,
   BREAK_SMOKE = 0x10,
   BREAK_TRANS = 0x20,
   BREAK_CONCRETE = 0x40,
   BREAK_2 = 0x80,

   BOUNCE_GLASS = BREAK_GLASS,
   BOUNCE_METAL = BREAK_METAL,
   BOUNCE_FLESH = BREAK_FLESH,
   BOUNCE_WOOD = BREAK_WOOD,
   BOUNCE_SHRAP = 0x10,
   BOUNCE_SHELL = 0x20,
   BOUNCE_CONCRETE = BREAK_CONCRETE,
   BOUNCE_SHOTSHELL = 0x80,

   AMBIENT_SOUND_STATIC = 0,
   AMBIENT_SOUND_EVERYWHERE = 1,
   AMBIENT_SOUND_SMALLRADIUS = 2,
   AMBIENT_SOUND_MEDIUMRADIUS = 4,
   AMBIENT_SOUND_LARGERADIUS = 8,
   AMBIENT_SOUND_START_SILENT = 16,
   AMBIENT_SOUND_NOT_LOOPING = 32,

   SND_STOP = (1 << 5),
   SND_CHANGE_VOL = (1 << 6),
   SND_CHANGE_PITCH = (1 << 7),

   SVC_SENDPINGS = 17,
   SVC_TEMPENTITY = 23,
   SVC_INTERMISSION = 30,
   SVC_CDTRACK = 32,
   SVC_WEAPONANIM = 35,
   SVC_ROOMTYPE = 37,
   SVC_DIRECTOR = 51,

   SPAWNFLAG_NOMESSAGE = 1,
   SPAWNFLAG_NOTOUCH = 1,
   SPAWNFLAG_DROIDONLY = 4,
   SPAWNFLAG_USEONLY = 1,

   MAX_ENT_LEAFS = 48,
   MAX_AMMO_TYPES = 32,
   MAX_AMMO_SLOTS = 32,

   HUD_PRINTNOTIFY = 1,
   HUD_PRINTCONSOLE = 2,
   HUD_PRINTTALK = 3,
   HUD_PRINTCENTER = 4,

   FCVAR_ARCHIVE = (1 << 0),
   FCVAR_USERINFO = (1 << 1),
   FCVAR_SERVER = (1 << 2),
   FCVAR_EXTDLL = (1 << 3),
   FCVAR_CLIENTDLL = (1 << 4),
   FCVAR_PROTECTED = (1 << 5),
   FCVAR_SPONLY = (1 << 6),
   FCVAR_PRINTABLEONLY = (1 << 7),
   FCVAR_UNLOGGED = (1 << 8),

   point_hull = 0,
   human_hull = 1,
   large_hull = 2,
   head_hull = 3,

   at_notice = 0,
   at_console,
   at_aiconsole,
   at_warning,
   at_error,
   at_logged,

   print_console = 0,
   print_center,
   print_chat,

   force_exactfile = 0,
   force_model_samebounds,
   force_model_specifybounds,

   kRenderNormal = 0,
   kRenderTransColor,
   kRenderTransTexture,
   kRenderGlow,
   kRenderTransAlpha,
   kRenderTransAdd,

   kRenderFxNone = 0,
   kRenderFxPulseSlow,
   kRenderFxPulseFast,
   kRenderFxPulseSlowWide,
   kRenderFxPulseFastWide,
   kRenderFxFadeSlow,
   kRenderFxFadeFast,
   kRenderFxSolidSlow,
   kRenderFxSolidFast,
   kRenderFxStrobeSlow,
   kRenderFxStrobeFast,
   kRenderFxStrobeFaster,
   kRenderFxFlickerSlow,
   kRenderFxFlickerFast,
   kRenderFxNoDissipation,
   kRenderFxDistort,
   kRenderFxHologram,
   kRenderFxDeadPlayer,
   kRenderFxExplode,
   kRenderFxGlowShell,
   kRenderFxClampMinScale,

   GINFO_NAME = 0,
   GINFO_DESC,
   GINFO_GAMEDIR,
   GINFO_DLL_FULLPATH,
   GINFO_DLL_FILENAME,
   GINFO_REALDLL_FULLPATH,

   PT_NEVER = 0,
   PT_STARTUP,
   PT_CHANGELEVEL,
   PT_ANYTIME,
   PT_ANYPAUSE,

   PNL_nullptr = 0,
   PNL_INI_DELETED,
   PNL_FILE_NEWER,
   PNL_COMMAND,
   PNL_CMD_FORCED,
   PNL_DELAYED,
   PNL_PLUGIN,
   PNL_PLG_FORCED,
   PNL_RELOAD,

   MRES_UNSET = 0,
   MRES_IGNORED,
   MRES_HANDLED,
   MRES_OVERRIDE,
   MRES_SUPERCEDE
};

#define VEC_HULL_MIN Vector(-16, -16, -36)
#define VEC_HULL_MAX Vector(16, 16, 36)
#define VEC_HUMAN_HULL_MIN Vector(-16, -16, 0)
#define VEC_HUMAN_HULL_MAX Vector(16, 16, 72)
#define VEC_HUMAN_HULL_DUCK Vector(16, 16, 36)

#define VEC_VIEW Vector(0, 0, 28)

#define VEC_DUCK_HULL_MIN Vector(-16, -16, -18)
#define VEC_DUCK_HULL_MAX Vector(16, 16, 18)
#define VEC_DUCK_VIEW Vector(0, 0, 12)
#define ATTN_NONE 0
#define ATTN_NORM (float)0.8
#define ATTN_IDLE (float)2
#define ATTN_STATIC (float)1.25

#define VOL_NORM 1.0
// max buffer size for printed messages
#define MAX_LOGMSG_LEN  1024
typedef struct link_s
{
   struct link_s *prev, *next;
} link_t;

struct edict_t;

typedef struct
{
   float time;
   float frametime;
   float force_retouch;
   sdk_string_t mapname;
   sdk_string_t startspot;
   float deathmatch;
   float coop;
   float teamplay;
   float serverflags;
   float found_secrets;
   Vector v_forward;
   Vector v_up;
   Vector v_right;
   float trace_allsolid;
   float trace_startsolid;
   float trace_fraction;
   Vector trace_endpos;
   Vector trace_plane_normal;
   float trace_plane_dist;
   edict_t *trace_ent;
   float trace_inopen;
   float trace_inwater;
   int trace_hitgroup;
   int trace_flags;
   int msg_entity;
   int cdAudioTrack;
   int maxClients;
   int maxEntities;
   const char *pStringBase;
   void *pSaveData;
   Vector vecLandmarkOffset;
} globalvars_t;


struct entvars_t
{
   sdk_string_t classname;
   sdk_string_t globalname;

   Vector origin;
   Vector oldorigin;
   Vector velocity;
   Vector basevelocity;
   Vector clbasevelocity;
   Vector movedir;
   Vector angles;
   Vector avelocity;
   Vector punchangle;
   Vector v_angle;
   Vector endpos;
   Vector startpos;

   float impacttime;
   float starttime;

   int fixangle;
   float idealpitch;
   float pitch_speed;
   float ideal_yaw;
   float yaw_speed;

   int modelindex;
   sdk_string_t model;

   int viewmodel;
   int weaponmodel;

   Vector absmin;
   Vector absmax;
   Vector mins;
   Vector maxs;
   Vector size;

   float ltime;
   float nextthink;

   int movetype;
   int solid;

   int skin;
   int body;
   int effects;

   float gravity;
   float friction;

   int light_level;
   int sequence;
   int gaitsequence;

   float frame;
   float animtime;
   float framerate;
   uint8 controller[4];
   uint8 blending[2];

   float scale;

   int rendermode;
   float renderamt;
   Vector rendercolor;
   int renderfx;

   float health;
   float frags;
   int weapons;
   float takedamage;

   int deadflag;
   Vector view_ofs;

   int button;
   int impulse;

   edict_t *chain;
   edict_t *dmg_inflictor;
   edict_t *enemy;
   edict_t *aiment;
   edict_t *owner;
   edict_t *groundentity;

   int spawnflags;
   int flags;

   int colormap;
   int team;

   float max_health;
   float teleport_time;
   float armortype;
   float armorvalue;
   int waterlevel;
   int watertype;

   sdk_string_t target;
   sdk_string_t targetname;
   sdk_string_t netname;
   sdk_string_t message;

   float dmg_take;
   float dmg_save;
   float dmg;
   float dmgtime;

   sdk_string_t noise;
   sdk_string_t noise1;
   sdk_string_t noise2;
   sdk_string_t noise3;

   float speed;
   float air_finished;
   float pain_finished;
   float radsuit_finished;

   edict_t *pContainingEntity;

   int playerclass;
   float maxspeed;

   float fov;
   int weaponanim;

   int pushmsec;
   int bInDuck;
   int flTimeStepSound;
   int flSwimTime;
   int flDuckTime;
   int iStepLeft;
   float flFallVelocity;

   int gamestate;
   int oldbuttons;
   int groupinfo;

   int iuser1;
   int iuser2;
   int iuser3;
   int iuser4;
   float fuser1;
   float fuser2;
   float fuser3;
   float fuser4;
   Vector vuser1;
   Vector vuser2;
   Vector vuser3;
   Vector vuser4;
   edict_t *euser1;
   edict_t *euser2;
   edict_t *euser3;
   edict_t *euser4;
};

struct edict_t
{
   int free;
   int serialnumber;
   link_t area;
   int headnode;
   int num_leafs;
   short leafnums[MAX_ENT_LEAFS];
   float freetime;
   void *pvPrivateData;
   entvars_t v;
};

struct cvar_t
{
   char *name;
   char *string;
   int flags;
   float value;
   cvar_t *next;
};

struct TraceResult
{
   int fAllSolid;
   int fStartSolid;
   int fInOpen;
   int fInWater;
   float flFraction;
   Vector vecEndPos;
   float flPlaneDist;
   Vector vecPlaneNormal;
   edict_t *pHit;
   int iHitgroup;
};

struct KeyValueData
{
   char *szClassName;
   char const *szKeyName;
   char *szValue;
   int32 fHandled;
};

struct plugin_info_t
{
   char const *ifvers;
   char const *name;
   char const *version;
   char const *date;
   char const *author;
   char const *url;
   char const *logtag;
   int loadable;
   int unloadable;
};

struct meta_globals_t
{
   int mres;
   int prev_mres;
   int status;
   void *orig_ret;
   void *override_ret;
};

struct enginefuncs_t
{
   int (*pfnPrecacheModel) (char *s);
   int (*pfnPrecacheSound) (char *s);
   void (*pfnSetModel) (edict_t *e, const char *m);
   int (*pfnModelIndex) (const char *m);
   int (*pfnModelFrames) (int modelIndex);
   void (*pfnSetSize) (edict_t *e, const float *rgflMin, const float *rgflMax);
   void (*pfnChangeLevel) (char *s1, char *s2);
   void (*pfnGetSpawnParms) (edict_t *ent);
   void (*pfnSaveSpawnParms) (edict_t *ent);
   float (*pfnVecToYaw) (const float *rgflVector);
   void (*pfnVecToAngles) (const float *rgflVectorIn, float *rgflVectorOut);
   void (*pfnMoveToOrigin) (edict_t *ent, const float *pflGoal, float dist, int iMoveType);
   void (*pfnChangeYaw) (edict_t *ent);
   void (*pfnChangePitch) (edict_t *ent);
   edict_t *(*pfnFindEntityByString) (edict_t *pentEdictStartSearchAfter, const char *pszField, const char *pszValue);
   int (*pfnGetEntityIllum) (edict_t *pEnt);
   edict_t *(*pfnFindEntityInSphere) (edict_t *pentEdictStartSearchAfter, const float *org, float rad);
   edict_t *(*pfnFindClientInPVS) (edict_t *ent);
   edict_t *(*pfnEntitiesInPVS) (edict_t *pplayer);
   void (*pfnMakeVectors) (const float *rgflVector);
   void (*pfnAngleVectors) (const float *rgflVector, float *forward, float *right, float *up);
   edict_t *(*pfnCreateEntity) (void);
   void (*pfnRemoveEntity) (edict_t *e);
   edict_t *(*pfnCreateNamedEntity) (int className);
   void (*pfnMakeStatic) (edict_t *ent);
   int (*pfnEntIsOnFloor) (edict_t *e);
   int (*pfnDropToFloor) (edict_t *e);
   int (*pfnWalkMove) (edict_t *ent, float yaw, float dist, int mode);
   void (*pfnSetOrigin) (edict_t *e, const float *rgflOrigin);
   void (*pfnEmitSound) (edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch);
   void (*pfnEmitAmbientSound) (edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch);
   void (*pfnTraceLine) (const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr);
   void (*pfnTraceToss) (edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr);
   int (*pfnTraceMonsterHull) (edict_t *ent, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr);
   void (*pfnTraceHull) (const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr);
   void (*pfnTraceModel) (const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr);
   const char *(*pfnTraceTexture) (edict_t *pTextureEntity, const float *v1, const float *v2);
   void (*pfnTraceSphere) (const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr);
   void (*pfnGetAimVector) (edict_t *ent, float speed, float *rgflReturn);
   void (*pfnServerCommand) (char *str);
   void (*pfnServerExecute) (void);
   void (*pfnClientCommand) (edict_t *ent, char const *szFmt, ...);
   void (*pfnParticleEffect) (const float *org, const float *dir, float color, float count);
   void (*pfnLightStyle) (int style, char *val);
   int (*pfnDecalIndex) (const char *name);
   int (*pfnPointContents) (const float *rgflVector);
   void (*pfnMessageBegin) (int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);
   void (*pfnMessageEnd) (void);
   void (*pfnWriteByte) (int value);
   void (*pfnWriteChar) (int value);
   void (*pfnWriteShort) (int value);
   void (*pfnWriteLong) (int value);
   void (*pfnWriteAngle) (float flValue);
   void (*pfnWriteCoord) (float flValue);
   void (*pfnWriteString) (const char *sz);
   void (*pfnWriteEntity) (int value);
   void (*pfnCVarRegister) (cvar_t *pCvar);
   float (*pfnCVarGetFloat) (const char *szVarName);
   const char *(*pfnCVarGetString) (const char *szVarName);
   void (*pfnCVarSetFloat) (const char *szVarName, float flValue);
   void (*pfnCVarSetString) (const char *szVarName, const char *szValue);
   void (*pfnAlertMessage) (int atype, char *szFmt, ...);
   void (*pfnEngineFprintf) (void *pfile, char *szFmt, ...);
   void *(*pfnPvAllocEntPrivateData) (edict_t *ent, int32 cb);
   void *(*pfnPvEntPrivateData) (edict_t *ent);
   void (*pfnFreeEntPrivateData) (edict_t *ent);
   const char *(*pfnSzFromIndex) (int stingPtr);
   int (*pfnAllostring) (const char *szValue);
   struct entvars_s *(*pfnGetVarsOfEnt) (edict_t *ent);
   edict_t *(*pfnPEntityOfEntOffset) (int iEntOffset);
   int (*pfnEntOffsetOfPEntity) (const edict_t *ent);
   int (*pfnIndexOfEdict) (const edict_t *ent);
   edict_t *(*pfnPEntityOfEntIndex) (int entIndex);
   edict_t *(*pfnFindEntityByVars) (struct entvars_s *pvars);
   void *(*pfnGetModelPtr) (edict_t *ent);
   int (*pfnRegUserMsg) (const char *pszName, int iSize);
   void (*pfnAnimationAutomove) (const edict_t *ent, float flTime);
   void (*pfnGetBonePosition) (const edict_t *ent, int iBone, float *rgflOrigin, float *rgflAngles);
   uint32 (*pfnFunctionFromName) (const char *pName);
   const char *(*pfnNameForFunction) (uint32 function);
   void (*pfnClientPrintf) (edict_t *ent, int ptype, const char *szMsg);
   void (*pfnServerPrint) (const char *szMsg);
   const char *(*pfnCmd_Args) (void);
   const char *(*pfnCmd_Argv) (int argc);
   int (*pfnCmd_Argc) (void);
   void (*pfnGetAttachment) (const edict_t *ent, int iAttachment, float *rgflOrigin, float *rgflAngles);
   void (*pfnCRC32_Init) (uint8 *pulCRC);
   void (*pfnCRC32_ProcessBuffer) (uint8 *pulCRC, void *p, int len);
   void (*pfnCRC32_ProcessByte) (uint8 *pulCRC, unsigned char ch);
   uint8 (*pfnCRC32_Final) (uint8 pulCRC);
   int32 (*pfnRandomLong) (int32 lLow, int32 lHigh);
   float (*pfnRandomFloat) (float flLow, float flHigh);
   void (*pfnSetView) (const edict_t *client, const edict_t *pViewent);
   float (*pfnTime) (void);
   void (*pfnCrosshairAngle) (const edict_t *client, float pitch, float yaw);
   uint8 *(*pfnLoadFileForMe) (char const *szFilename, int *pLength);
   void (*pfnFreeFile) (void *buffer);
   void (*pfnEndSection) (const char *pszSectionName);
   int (*pfnCompareFileTime) (char *filename1, char *filename2, int *compare);
   void (*pfnGetGameDir) (char *szGetGameDir);
   void (*pfnCvar_RegisterVariable) (cvar_t *variable);
   void (*pfnFadeClientVolume) (const edict_t *ent, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds);
   void (*pfnSetClientMaxspeed) (const edict_t *ent, float fNewMaxspeed);
   edict_t *(*pfnCreateFakeClient) (const char *netname);
   void (*pfnRunPlayerMove) (edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, uint8 impulse, uint8 msec);
   int (*pfnNumberOfEntities) (void);
   char *(*pfnGetInfoKeyBuffer) (edict_t *e);
   char *(*pfnInfoKeyValue) (char *infobuffer, char const *key);
   void (*pfnSetKeyValue) (char *infobuffer, char *key, char *value);
   void (*pfnSetClientKeyValue) (int clientIndex, char *infobuffer, char const *key, char const *value);
   int (*pfnIsMapValid) (char *szFilename);
   void (*pfnStaticDecal) (const float *origin, int decalIndex, int entityIndex, int modelIndex);
   int (*pfnPrecacheGeneric) (char *s);
   int (*pfnGetPlayerUserId) (edict_t *e);
   void (*pfnBuildSoundMsg) (edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);
   int (*pfnIsDedicatedServer) (void);
   cvar_t    *(*pfnCVarGetPointer) (const char *szVarName);
   unsigned int (*pfnGetPlayerWONId) (edict_t *e);
   void (*pfnInfo_RemoveKey) (char *s, const char *key);
   const char *(*pfnGetPhysicsKeyValue) (const edict_t *client, const char *key);
   void (*pfnSetPhysicsKeyValue) (const edict_t *client, const char *key, const char *value);
   const char *(*pfnGetPhysicsInfoString) (const edict_t *client);
   unsigned short (*pfnPrecacheEvent) (int type, const char *psz);
   void (*pfnPlaybackEvent) (int flags, const edict_t *pInvoker, unsigned short evIndexOfEntity, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
   unsigned char *(*pfnSetFatPVS) (float *org);
   unsigned char *(*pfnSetFatPAS) (float *org);
   int (*pfnCheckVisibility) (const edict_t *entity, unsigned char *pset);
   void (*pfnDeltaSetField) (struct delta_s *pFields, const char *fieldname);
   void (*pfnDeltaUnsetField) (struct delta_s *pFields, const char *fieldname);
   void (*pfnDeltaAddEncoder) (char *name, void (*conditionalencode) (struct delta_s *pFields, const unsigned char *from, const unsigned char *to));
   int (*pfnGetCurrentPlayer) (void);
   int (*pfnCanSkipPlayer) (const edict_t *player);
   int (*pfnDeltaFindField) (struct delta_s *pFields, const char *fieldname);
   void (*pfnDeltaSetFieldByIndex) (struct delta_s *pFields, int fieldNumber);
   void (*pfnDeltaUnsetFieldByIndex) (struct delta_s *pFields, int fieldNumber);
   void (*pfnSetGroupMask) (int mask, int op);
   int (*pfnCreateInstancedBaseline) (int classname, struct entity_state_s *baseline);
   void (*pfnCvar_DirectSet) (struct cvar_t *var, char *value);
   void (*pfnForceUnmodified) (int type, float *mins, float *maxs, const char *szFilename);
   void (*pfnGetPlayerStats) (const edict_t *client, int *ping, int *packet_loss);
   void (*pfnAddServerCommand) (char *cmd_name, void (*function) (void));
   int (*pfnVoice_GetClientListening) (int iReceiver, int iSender);
   int (*pfnVoice_SetClientListening) (int iReceiver, int iSender, int bListen);
   const char *(*pfnGetPlayerAuthId) (edict_t *e);
   struct sequenceEntry_s *(*pfnSequenceGet) (const char *fileName, const char *entryName);
   struct sentenceEntry_s *(*pfnSequencePickSentence) (const char *groupName, int pickMethod, int *picked);
   int (*pfnGetFileSize) (char *szFilename);
   unsigned int (*pfnGetApproxWavePlayLen) (const char *filepath);
   int (*pfnIsCareerMatch) (void);
   int (*pfnGetLocalizedStringLength) (const char *label);
   void (*pfnRegisterTutorMessageShown) (int mid);
   int (*pfnGetTimesTutorMessageShown) (int mid);
   void (*pfnProcessTutorMessageDecayBuffer) (int *buffer, int bufferLength);
   void (*pfnConstructTutorMessageDecayBuffer) (int *buffer, int bufferLength);
   void (*pfnResetTutorMessageDecayData) (void);
   void (*pfnQueryClientCVarValue) (const edict_t *player, const char *cvarName);
   void (*pfnQueryClientCVarValue2) (const edict_t *player, const char *cvarName, int requestID);
   int (*pfnCheckParm)(const char *pchCmdLineToken, char **ppnext);
};

struct gamefuncs_t
{
   void (*pfnGameInit)(void);
   int (*pfnSpawn)(edict_t *pent);
   void (*pfnThink)(edict_t *pent);
   void (*pfnUse)(edict_t *pentUsed, edict_t *pentOther);
   void (*pfnTouch)(edict_t *pentTouched, edict_t *pentOther);
   void (*pfnBlocked)(edict_t *pentBlocked, edict_t *pentOther);
   void (*pfnKeyValue)(edict_t *pentKeyvalue, KeyValueData *pkvd);
   void (*pfnSave)(edict_t *pent, struct SAVERESTOREDATA *pSaveData);
   int (*pfnRestore)(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
   void (*pfnSetAbsBox)(edict_t *pent);
   void (*pfnSaveWriteFields)(SAVERESTOREDATA *, const char *, void *, struct TYPEDESCRIPTION *, int);
   void (*pfnSaveReadFields)(SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);
   void (*pfnSaveGlobalState)(SAVERESTOREDATA *);
   void (*pfnRestoreGlobalState)(SAVERESTOREDATA *);
   void (*pfnResetGlobalState)(void);
   int (*pfnClientConnect)(edict_t *ent, const char *pszName, const char *pszAddress, char szRejectReason[128]);
   void (*pfnClientDisconnect)(edict_t *ent);
   void (*pfnClientKill)(edict_t *ent);
   void (*pfnClientPutInServer)(edict_t *ent);
   void (*pfnClientCommand)(edict_t *ent);
   void (*pfnClientUserInfoChanged)(edict_t *ent, char *infobuffer);
   void (*pfnServerActivate)(edict_t *edictList, int edictCount, int clientMax);
   void (*pfnServerDeactivate)(void);
   void (*pfnPlayerPreThink)(edict_t *ent);
   void (*pfnPlayerPostThink)(edict_t *ent);
   void (*pfnStartFrame)(void);
   void (*pfnParmsNewLevel)(void);
   void (*pfnParmsChangeLevel)(void);
   const char *(*pfnGetGameDescription)(void);
   void (*pfnPlayerCustomization)(edict_t *ent, struct customization_s *pCustom);
   void (*pfnSpectatorConnect)(edict_t *ent);
   void (*pfnSpectatorDisconnect)(edict_t *ent);
   void (*pfnSpectatorThink)(edict_t *ent);
   void (*pfnSys_Error)(const char *error_string);
   void (*pfnPM_Move)(struct playermove_s *ppmove, int server);
   void (*pfnPM_Init)(struct playermove_s *ppmove);
   char (*pfnPM_FindTextureType)(char *name);
   void (*pfnSetupVisibility)(struct edict_s *pViewEntity, struct edict_s *client, uint8 **pvs, uint8 **pas);
   void (*pfnUpdateClientData)(const struct edict_t *ent, int sendweapons, struct clientdata_s *cd);
   int (*pfnAddToFullPack)(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, uint8 *pSet);
   void (*pfnCreateBaseline)(int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, float *player_mins, float *player_maxs);
   void (*pfnRegisterEncoders)(void);
   int (*pfnGetWeaponData)(struct edict_s *player, struct weapon_data_s *info);
   void (*pfnCmdStart)(const edict_t *player, const struct c *cmd, unsigned int random_seed);
   void (*pfnCmdEnd)(const edict_t *player);
   int (*pfnConnectionlessPacket)(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
   int (*pfnGetHullBounds)(int hullnumber, float *mins, float *maxs);
   void (*pfnCreateInstancedBaselines)(void);
   int (*pfnInconsistentFile)(const struct edict_s *player, const char *szFilename, char *disconnect_message);
   int (*pfnAllowLagCompensation)(void);
};

struct newgamefuncs_t
{
   void (*pfnOnFreeEntPrivateData)(edict_t *pEnt);
   void (*pfnGameShutdown)(void);
   int (*pfnShouldCollide)(edict_t *pentTouched, edict_t *pentOther);

   void (*pfnCvarValue)(const edict_t *pEnt, const char *value);
   void (*pfnCvarValue2)(const edict_t *pEnt, int requestID, const char *cvarName, const char *value);
};

struct mutil_funcs_t
{
   void (*pfnLogConsole) (plugin_info_t *plid, const char *szFormat, ...);
   void (*pfnLogMessage) (plugin_info_t *plid, const char *szFormat, ...);
   void (*pfnLogError)   (plugin_info_t *plid, const char *szFormat, ...);
   void (*pfnLogDeveloper) (plugin_info_t *plid, const char *szFormat, ...);
   void (*pfnCenterSay) (plugin_info_t *plid, const char *szFormat, ...);
   void (*pfnCenterSayParms)   (plugin_info_t *plid, void* tparms, const char *szFormat, ...);
   void (*pfnCenterSayVarargs) (plugin_info_t *plid, void* tparms, const char *szFormat, va_list ap);
   int (*pfnCallGameEntity) (plugin_info_t *plid, const char *entStr, entvars_t *pev);
   int (*pfnGetUserMsgID) (plugin_info_t *plid, const char *msgname, int *size);
   const char *(*pfnGetUserMsgName) (plugin_info_t *plid, int msgid, int *size);
   const char *(*pfnGetPluginPath) (plugin_info_t *plid);
   const char *(*pfnGetGameInfo) (plugin_info_t *plid, int tag);
   int (*pfnLoadPlugin) (plugin_info_t *plid, const char *cmdline, int now, void **plugin_handle);
   int (*pfnUnloadPlugin) (plugin_info_t *plid, const char *cmdline, int now, int reason);
   int (*pfnUnloadPluginByHandle) (plugin_info_t *plid, void *plugin_handle, int now, int reason);
   const char *(*pfnIsQueryingClienCVar_t)   (plugin_info_t *plid, const edict_t *player);
   int (*pfnMakeRequestID) (plugin_info_t *plid);
   void (*pfnGetHookTables) (plugin_info_t *plid, enginefuncs_t **peng, gamefuncs_t **pdll, newgamefuncs_t **pnewdll);
};

struct gamedll_funcs_t
{
   gamefuncs_t *dllapi_table;
   newgamefuncs_t *newapi_table;
};

typedef int (*GETENTITYAPI_FN) (gamefuncs_t *, int );
typedef int (*GETENTITYAPI2_FN) (gamefuncs_t *, int *);

typedef int (*GETNEWDLLFUNCTIONS_FN) (newgamefuncs_t *, int *);
typedef int (*GETENGINEFUNCTIONS_FN) (enginefuncs_t *, int *);
typedef int (*GETBLENDINGINTERFACE_FN) (int, void **, void *, float (*)[3][4], float (*)[128][3][4]);

typedef void (*PASSGAMEENTITY_FN) (entvars_t *);
typedef void (STDCALL *GIVEFNPTRSTODLL_FN) (enginefuncs_t *, globalvars_t *);

#define META_INTERFACE_VERSION "5:13"
#define GAME_INTERFACE_VERSION 140

typedef struct
{
   GETENTITYAPI_FN pfnGetEntityAPI;
   GETENTITYAPI_FN pfnGetEntityAPI_Post;
   GETENTITYAPI2_FN pfnGetEntityAPI2;
   GETENTITYAPI2_FN pfnGetEntityAPI2_Post;
   GETNEWDLLFUNCTIONS_FN pfnGetNewDLLFunctions;
   GETNEWDLLFUNCTIONS_FN pfnGetNewDLLFunctions_Post;
   GETENGINEFUNCTIONS_FN pfnGetEngineFunctions;
   GETENGINEFUNCTIONS_FN pfnGetEngineFunctions_Post;
} metamod_funcs_t;

extern enginefuncs_t g_engfuncs;
extern globalvars_t *g_pGlobals;

class Entity {
public:
   Entity () : pev (NULL) {}
   Entity (edict_t *pEdict) : pev (&pEdict->v) {}
   virtual ~Entity () {}

   entvars_t    *pev;

   // From Half-Life SDK cbase.h:
   inline edict_t   *edict () { return pev->pContainingEntity; }
   int        entindex ();

   inline bool operator==(Entity &e) { return entindex () == e.entindex (); }

   static Entity *Instance (edict_t *pent);
   static Entity *Instance (entvars_t *pev) { return Instance (pev->pContainingEntity); }

   bool   IsValid (void);
   inline bool   IsMonster (void) { return (pev->flags & FL_MONSTER) != 0; }
   inline bool   IsPlayer (void) { return (pev->flags & FL_CLIENT) != 0 && entindex () <= g_pGlobals->maxClients; }

   inline bool   HasBoundingBox (void)
   {
      // this function returns TRUE if entity has a valid bounding box, FALSE otherwise
      return (!Math::FltZero (pev->absmin.x) || !Math::FltZero (pev->absmin.y) || !Math::FltZero (pev->absmin.z) ||
         !Math::FltZero (pev->absmax.x) || !Math::FltZero (pev->absmax.y) || !Math::FltZero (pev->absmax.z));
   }

   inline Vector GetAbsMin (void) { return pev->absmin; }
   inline Vector GetAbsMax (void) { return pev->absmax; }
   inline Vector GetMins (void) { return pev->mins; }
   inline Vector GetMaxs (void) { return pev->maxs; }

   const char  *GetClassname (void);
   const char  *GetNetName (void);
   const char  *GetModel (void);
   const char  *GetViewModel (void);
   inline int    GetHealth (void) { return (int) pev->health; }
   inline Vector Center (void) { return (pev->absmax + pev->absmin) * 0.5; } // center point of entity
   inline Vector GetOrigin (void) { return HasBoundingBox () ? Center () : pev->origin; }
   inline Vector EyePosition (void) { return pev->origin + pev->view_ofs; } // position of eyes
   inline Vector EarPosition (void) { return pev->origin + pev->view_ofs; } // position of ears
   inline Vector GetGunPosition (void) { return pev->origin + pev->view_ofs; }
   inline Vector GetVelocity (void) { return pev->velocity; }
   inline float  GetSpeed (void) { return pev->velocity.GetLength (); }
   inline float  GetSpeed2D (void) { return pev->velocity.GetLength2D (); }
   inline float  GetArmor (void) { return pev->armorvalue; }
   inline float  GetArmorType (void) { return pev->armortype; }
   inline float  GetMaxspeed (void) { return pev->maxspeed; }
   inline Vector GetBodyAngles (void) { return pev->angles; }
   inline Vector GetViewAngles (void) { return pev->v_angle; }
   inline float  GetFov (void) { return pev->fov; }

   inline int    GetMoveType (void) { return pev->movetype; }
   inline int    GetSpawnFlags (void) { return pev->spawnflags; }
   inline int    GetFlags (void) { return pev->flags; }
   inline int    GetWaterLevel (void) { return pev->waterlevel; }

   const char *GetTargetName (void);
   const char *GetTarget (void);

   inline void  ReleaseAllButtons (void) { pev->button = 0; }
   inline void  PushButton (int iButton) { pev->button |= iButton; }
   inline void  ReleaseButton (int iButton) { pev->button &= ~iButton; }
   inline int   GetButtons (void) { return pev->button; }

   inline void  SetAngles (const Vector &vecViewAngles)
   {
      pev->angles = pev->v_angle = vecViewAngles;
      pev->angles.x /= -3;
      pev->angles.ClampAngles ();
      pev->v_angle.ClampAngles ();
   }

   inline int    GetWeaponAnim (void) { return pev->weaponanim; }
   inline int    GetWeapons (void) { return pev->weapons; }

   inline Entity *GetOwner (void) { return Instance (pev->owner); }
   inline Entity *GetGroundEntity (void) { return Instance (pev->groundentity); }

   inline bool   IsVisible (void) { return !(pev->effects & EF_NODRAW) && GetModel ()[0]; }
   inline bool   IsAlive (void) { return pev->deadflag == DEAD_NO && GetHealth () > 0 && !(pev->flags & FL_NOTARGET); }
   inline bool   IsOnLadder (void) { return pev->movetype == MOVETYPE_FLY; }
   inline bool   IsInWater (void) { return pev->waterlevel >= 2; }
   inline bool   IsOnFloor (void) { return IsPlayer () ? ((pev->flags & (FL_ONGROUND | FL_PARTIALGROUND)) != 0) : (g_engfuncs.pfnEntIsOnFloor (edict ()) != 0); }
   inline bool   IsMoving (void) { return (GetSpeed () > 0); }
   inline bool   IsDucking (void) { return (pev->flags & FL_DUCKING) != 0; }

   bool          FInViewCone (Vector *pOrigin, float fov = 0);
   bool          FInViewCone (Entity *pEntity, float fov = 0);

   bool          FVisible (const Vector &vecDest);
   bool          FVisible (Entity *pEntity);

   bool          FBoxVisible (Entity *pEntity, Vector *pvHit = NULL, unsigned char *ucBodyPart = NULL);

   inline void   SetSpawnFlags (int flag) { pev->spawnflags |= flag; }
   inline void   SetFlags (int flag) { pev->flags |= flag; }
   inline void   SetBodyAngles (const Vector &a) { pev->angles = a; }
   inline void   SetViewAngles (const Vector &a) { pev->v_angle = a; }

   void          ChangeAngles (const Vector &idealangles, float speed = 1.0); // speed: 0.1 - 1
};




// multiloader (standalone & metamod)
class GoldSrcLoader : public Singleton <GoldSrcLoader>
{
protected:
   plugin_info_t m_pluginInfo;
   bool m_isMetamod;

   gamedll_funcs_t *m_metaGameFunctions;
   mutil_funcs_t *m_metaUtilityFunctions;
   meta_globals_t *m_metaGlobals;

   metamod_funcs_t m_metaFunctionTable;
   gamedll_funcs_t m_metaFakeFunctions;
   gamefuncs_t m_dllapiFunctions;

   class IGameEvents *m_events;
   edict_t *m_killerEntity;

public:
   FORCEINLINE void SetEventListener (IGameEvents *events)
   {
      m_events = events;
   }

   FORCEINLINE IGameEvents *operator -> (void)
   {
      return m_events;
   }

   FORCEINLINE bool IsMetamod (void)
   {
      return m_isMetamod;
   }

   FORCEINLINE gamefuncs_t *GetDllapiFunctionTable (void)
   {
      return &m_dllapiFunctions;
   }
   
   FORCEINLINE void SetMetaResult (int mres)
   {
      m_metaGlobals->mres = mres;
   }

public:
   // wrap metamod functions
   int Meta_GetUserMsgId (const char *msgname);
   int CallPlayerEntity (entvars_t *pev);
   
   // initializers
public:
   void ExecuteEntityAPIs (gamefuncs_t *table);
   void ExecuteMetaQuery (plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs);
   void ExecuteMetaAttach (metamod_funcs_t *table, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs);
   int  ExecuteNewEntityAPIs (newgamefuncs_t *table, int version);

public:
   void CreateKillerEntity (void);
   void DestroyKillerEntity (void);
   void SlayPlayer (edict_t *ent, const char *weapon);
   void SendClientCommands (edict_t *ent);
   void SendUse (edict_t *what, edict_t *who);

   edict_t *ConstructFakeClient (const char *botName, int pingType, bool avatars, const char *steamID);

public:
   void SetupMetamod (void);
};

// simple valve string wrapper
class ValveString
{
public:
   FORCEINLINE static const char *Get (int offset)
   {
      return (g_pGlobals->pStringBase + offset);
   }

   FORCEINLINE static int Make (const char *str)
   {
      return (str - Get (0));
   }

   FORCEINLINE static  char *Alloc (const char *str)
   {
      return const_cast <char *> (Get (g_engfuncs.pfnAllostring (str)));
   }
};

// wrapper for goldsrc messages (temporary)
class MessageWriter
{
public:
   static inline uint16 FixedUnsigned16 (float value, float scale)
   {
      int output = (static_cast <int> (value * scale));

      if (output < 0)
         output = 0;

      if (output > 0xffff)
         output = 0xffff;

      return static_cast <uint16> (output);
   }

   static inline short FixedSigned16 (float value, float scale)
   {
      int output = (static_cast <int> (value * scale));

      if (output > 32767)
         output = 32767;

      if (output < -32768)
         output = -32768;

      return static_cast <short> (output);
   }

public:
   FORCEINLINE static MessageWriter &Begin (int dest, int type, const float *origin = nullptr, edict_t *ent = nullptr)
   {
      static MessageWriter msg;
      g_engfuncs.pfnMessageBegin (dest, type, origin, ent);

      return msg;
   }

   FORCEINLINE void End (void)
   {
      g_engfuncs.pfnMessageEnd ();
   }

   FORCEINLINE MessageWriter &Byte (int val)
   {
      g_engfuncs.pfnWriteByte (val);
      return *this;
   }

   FORCEINLINE MessageWriter &Short (int val)
   {
      g_engfuncs.pfnWriteShort (val);
      return *this;
   }

   FORCEINLINE MessageWriter &Coord (float val)
   {
      g_engfuncs.pfnWriteCoord (val);
      return *this;
   }

   FORCEINLINE MessageWriter &Char (int val)
   {
      g_engfuncs.pfnWriteChar (val);
      return *this;
   }

   FORCEINLINE MessageWriter &Str (const char *val)
   {
      g_engfuncs.pfnWriteString (val);
      return *this;
   }
};

// goldsrc instance
extern GoldSrcLoader gs;

#endif // EXTDLL_H
