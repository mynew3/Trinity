/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2013 Project Cerberus <http://www.erabattle.ru/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is not free software; you can not redistribute it and/or modify it.
 *
 * This program is distributed only by <http://www.erabattle.ru/>!
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "blackwing_lair.h"

enum Emotes
{
    EMOTE_FRENZY            = 0,
};

enum Spells
{
    SPELL_SHADOWFLAME        = 22539,
    SPELL_WINGBUFFET         = 23339,
    SPELL_FRENZY             = 23342  //This spell periodically triggers fire nova
};

enum Events
{
    EVENT_SHADOWFLAME       = 1,
    EVENT_WINGBUFFET        = 2,
    EVENT_FRENZY            = 3
};

class boss_flamegor : public CreatureScript
{
public:
    boss_flamegor() : CreatureScript("boss_flamegor") { }

    struct boss_flamegorAI : public BossAI
    {
        boss_flamegorAI(Creature* creature) : BossAI(creature, BOSS_FLAMEGOR) { }

        void EnterCombat(Unit* /*who*/) OVERRIDE
        {
            if (instance && instance->GetBossState(BOSS_BROODLORD) != DONE)
            {
                EnterEvadeMode();
                return;
            }
            _EnterCombat();

            events.ScheduleEvent(EVENT_SHADOWFLAME, urand(10000, 20000));
            events.ScheduleEvent(EVENT_WINGBUFFET, 30000);
            events.ScheduleEvent(EVENT_FRENZY, 10000);
        }

        void UpdateAI(uint32 diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SHADOWFLAME:
                        DoCastVictim(SPELL_SHADOWFLAME);
                        events.ScheduleEvent(EVENT_SHADOWFLAME, urand(10000, 20000));
                        break;
                    case EVENT_WINGBUFFET:
                        DoCastVictim(SPELL_WINGBUFFET);
                        if (DoGetThreat(me->GetVictim()))
                            DoModifyThreatPercent(me->GetVictim(), -75);
                        events.ScheduleEvent(EVENT_WINGBUFFET, 30000);
                        break;
                    case EVENT_FRENZY:
                        Talk(EMOTE_FRENZY);
                        DoCast(me, SPELL_FRENZY);
                        events.ScheduleEvent(EVENT_FRENZY, urand(8000, 10000));
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new boss_flamegorAI(creature);
    }
};

void AddSC_boss_flamegor()
{
    new boss_flamegor();
}
