# 1 "data//maps/LittlerootTown/scripts.pory"

# 2 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_MapScripts::
# 3 "data//maps/LittlerootTown/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, LittlerootTown_OnTransition
# 4 "data//maps/LittlerootTown/scripts.pory"
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LittlerootTown_OnFrame
# 5 "data//maps/LittlerootTown/scripts.pory"
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, LittlerootTown_OnWarp
# 6 "data//maps/LittlerootTown/scripts.pory"
	.byte 0
# 7 "data//maps/LittlerootTown/scripts.pory"

# 8 "data//maps/LittlerootTown/scripts.pory"
	@ State descriptions for VAR_LITTLEROOT_TOWN_STATE
# 9 "data//maps/LittlerootTown/scripts.pory"
	@ 1: Met Rival
# 10 "data//maps/LittlerootTown/scripts.pory"
	@ 2: Saving Birch
# 11 "data//maps/LittlerootTown/scripts.pory"
	@ 3: Received Pokedex
# 12 "data//maps/LittlerootTown/scripts.pory"
	@ 4: Received Running Shoes
# 13 "data//maps/LittlerootTown/scripts.pory"

# 14 "data//maps/LittlerootTown/scripts.pory"
	@ State descriptions for VAR_LITTLEROOT_INTRO_STATE
# 15 "data//maps/LittlerootTown/scripts.pory"
	@ 1: In truck male
# 16 "data//maps/LittlerootTown/scripts.pory"
	@ 2: In truck female
# 17 "data//maps/LittlerootTown/scripts.pory"
	@ 3: Entered house
# 18 "data//maps/LittlerootTown/scripts.pory"
	@ 4: Told to go set clock
# 19 "data//maps/LittlerootTown/scripts.pory"
	@ 5: Entered 2F to set clock
# 20 "data//maps/LittlerootTown/scripts.pory"
	@ 6: Set clock
# 21 "data//maps/LittlerootTown/scripts.pory"
	@ 7: Told to go meet rival
# 22 "data//maps/LittlerootTown/scripts.pory"

# 23 "data//maps/LittlerootTown/scripts.pory"
	@ State descriptions for VAR_LITTLEROOT_HOUSES_STATE_MAY
# 24 "data//maps/LittlerootTown/scripts.pory"
	@                    and VAR_LITTLEROOT_HOUSES_STATE_BRENDAN
# 25 "data//maps/LittlerootTown/scripts.pory"
	@ 1: On Truck (and is corresponding gender)
# 26 "data//maps/LittlerootTown/scripts.pory"
	@ 2: Met Rival's Mom (and is corresponding gender)
# 27 "data//maps/LittlerootTown/scripts.pory"
	@ 3: Entered Hall of Fame
# 28 "data//maps/LittlerootTown/scripts.pory"
	@ 4: Completed Lati TV / SS Ticket event
# 29 "data//maps/LittlerootTown/scripts.pory"
	@ 5: Exited house after above event
# 30 "data//maps/LittlerootTown/scripts.pory"

# 31 "data//maps/LittlerootTown/scripts.pory"
	@ State descriptions for VAR_LITTLEROOT_RIVAL_STATE
# 32 "data//maps/LittlerootTown/scripts.pory"
	@ 1: Never occurs
# 33 "data//maps/LittlerootTown/scripts.pory"
	@ 2: Entered Rival's bedroom
# 34 "data//maps/LittlerootTown/scripts.pory"
	@ 3: Met Rival
# 35 "data//maps/LittlerootTown/scripts.pory"
	@ 4: Received Pokedex
# 36 "data//maps/LittlerootTown/scripts.pory"

# 37 "data//maps/LittlerootTown/scripts.pory"

# 38 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_OnTransition:
# 39 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_VISITED_LITTLEROOT_TOWN
# 40 "data//maps/LittlerootTown/scripts.pory"
	call Common_EventScript_SetupRivalGfxId
# 41 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 2, LittlerootTown_EventScript_MoveMomToMaysDoor
# 42 "data//maps/LittlerootTown/scripts.pory"
	call_if_unset FLAG_RESCUED_BIRCH, LittlerootTown_EventScript_SetTwinPos
# 43 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_LITTLEROOT_TOWN_STATE, 3, LittlerootTown_EventScript_SetMomStandingInFrontOfDoorPos
# 44 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_LITTLEROOT_HOUSES_STATE_MAY, 4, LittlerootTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent
# 45 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_LITTLEROOT_HOUSES_STATE_BRENDAN, 4, LittlerootTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent
# 46 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_OLDALE_RIVAL_STATE, 1, LittlerootTown_EventScript_MoveRivalFromOldale
# 47 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_LITTLEROOT_RIVAL_STATE, 3, LittlerootTown_EventScript_SetRivalLeftForRoute103
# 48 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, LittlerootTown_EventScript_HideMapNamePopup
# 49 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2, LittlerootTown_EventScript_LeftLabAfterDexUpgrade
# 50 "data//maps/LittlerootTown/scripts.pory"
	end
# 51 "data//maps/LittlerootTown/scripts.pory"

# 52 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_LeftLabAfterDexUpgrade::
# 53 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3
# 54 "data//maps/LittlerootTown/scripts.pory"
	return
# 55 "data//maps/LittlerootTown/scripts.pory"

# 56 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_HideMapNamePopup::
# 57 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_HIDE_MAP_NAME_POPUP
# 58 "data//maps/LittlerootTown/scripts.pory"
	return
# 59 "data//maps/LittlerootTown/scripts.pory"

# 60 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetRivalLeftForRoute103::
# 61 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_RIVAL_LEFT_FOR_ROUTE103
# 62 "data//maps/LittlerootTown/scripts.pory"
	return
# 63 "data//maps/LittlerootTown/scripts.pory"

# 64 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MoveRivalFromOldale::
# 65 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_OLDALE_RIVAL_STATE, 2
# 66 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_HIDE_OLDALE_TOWN_RIVAL
# 67 "data//maps/LittlerootTown/scripts.pory"
	return
# 68 "data//maps/LittlerootTown/scripts.pory"

# 69 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent::
# 70 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_LITTLEROOT_HOUSES_STATE_MAY, 5
# 71 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_LITTLEROOT_HOUSES_STATE_BRENDAN, 5
# 72 "data//maps/LittlerootTown/scripts.pory"
	return
# 73 "data//maps/LittlerootTown/scripts.pory"

# 74 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MoveMomToMaysDoor::
# 75 "data//maps/LittlerootTown/scripts.pory"
	setobjectxyperm LOCALID_LITTLEROOT_MOM, 14, 8
# 76 "data//maps/LittlerootTown/scripts.pory"
	return
# 77 "data//maps/LittlerootTown/scripts.pory"

# 78 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetTwinPos::
# 79 "data//maps/LittlerootTown/scripts.pory"
	goto_if_eq VAR_LITTLEROOT_TOWN_STATE, 0, LittlerootTown_EventScript_SetTwinGuardingRoutePos
# 80 "data//maps/LittlerootTown/scripts.pory"
	setobjectxyperm LOCALID_LITTLEROOT_TWIN, 10, 1
# 81 "data//maps/LittlerootTown/scripts.pory"
	setobjectmovementtype LOCALID_LITTLEROOT_TWIN, MOVEMENT_TYPE_FACE_UP
# 82 "data//maps/LittlerootTown/scripts.pory"
	return
# 83 "data//maps/LittlerootTown/scripts.pory"

# 84 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetTwinGuardingRoutePos::
# 85 "data//maps/LittlerootTown/scripts.pory"
	setobjectxyperm LOCALID_LITTLEROOT_TWIN, 7, 2
# 86 "data//maps/LittlerootTown/scripts.pory"
	setobjectmovementtype LOCALID_LITTLEROOT_TWIN, MOVEMENT_TYPE_FACE_DOWN
# 87 "data//maps/LittlerootTown/scripts.pory"
	return
# 88 "data//maps/LittlerootTown/scripts.pory"

# 89 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetMomStandingInFrontOfDoorPos::
# 90 "data//maps/LittlerootTown/scripts.pory"
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MOM_OUTSIDE
# 91 "data//maps/LittlerootTown/scripts.pory"
	setobjectmovementtype LOCALID_LITTLEROOT_MOM, MOVEMENT_TYPE_FACE_DOWN
# 92 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 93 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_SetMomInFrontOfDoorMale
# 94 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_SetMomInFrontOfDoorFemale
# 95 "data//maps/LittlerootTown/scripts.pory"
	return
# 96 "data//maps/LittlerootTown/scripts.pory"

# 97 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetMomInFrontOfDoorMale::
# 98 "data//maps/LittlerootTown/scripts.pory"
	setobjectxyperm LOCALID_LITTLEROOT_MOM, 5, 9
# 99 "data//maps/LittlerootTown/scripts.pory"
	return
# 100 "data//maps/LittlerootTown/scripts.pory"

# 101 "data//maps/LittlerootTown/scripts.pory"
Speaker_Jeremy:
# 102 "data//maps/LittlerootTown/scripts.pory"
    .string "Jeremy$"
# 103 "data//maps/LittlerootTown/scripts.pory"

# 104 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetMomInFrontOfDoorFemale::
# 105 "data//maps/LittlerootTown/scripts.pory"
	setobjectxyperm LOCALID_LITTLEROOT_MOM, 14, 9
# 106 "data//maps/LittlerootTown/scripts.pory"
	return
# 107 "data//maps/LittlerootTown/scripts.pory"

# 108 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_OnFrame:
# 109 "data//maps/LittlerootTown/scripts.pory"
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 1, LittlerootTown_EventScript_StepOffTruckMale
# 110 "data//maps/LittlerootTown/scripts.pory"
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 2, LittlerootTown_EventScript_StepOffTruckFemale
# 111 "data//maps/LittlerootTown/scripts.pory"
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, LittlerootTown_EventScript_BeginDexUpgradeScene
# 112 "data//maps/LittlerootTown/scripts.pory"
	.2byte 0
# 113 "data//maps/LittlerootTown/scripts.pory"

# 114 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_StepOffTruckMale::
# 115 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 116 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8004, 5
# 117 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8005, 8
# 118 "data//maps/LittlerootTown/scripts.pory"
	call LittlerootTown_EventScript_GoInsideWithMom
# 119 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_TRUCK
# 120 "data//maps/LittlerootTown/scripts.pory"
	warpsilent MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F, 8, 8
# 121 "data//maps/LittlerootTown/scripts.pory"
	waitstate
# 122 "data//maps/LittlerootTown/scripts.pory"
	releaseall
# 123 "data//maps/LittlerootTown/scripts.pory"
	end
# 124 "data//maps/LittlerootTown/scripts.pory"

# 125 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_StepOffTruckFemale::
# 126 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 127 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8004, 14
# 128 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8005, 8
# 129 "data//maps/LittlerootTown/scripts.pory"
	call LittlerootTown_EventScript_GoInsideWithMom
# 130 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_TRUCK
# 131 "data//maps/LittlerootTown/scripts.pory"
	warpsilent MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F, 2, 8
# 132 "data//maps/LittlerootTown/scripts.pory"
	waitstate
# 133 "data//maps/LittlerootTown/scripts.pory"
	releaseall
# 134 "data//maps/LittlerootTown/scripts.pory"
	end
# 135 "data//maps/LittlerootTown/scripts.pory"

# 136 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GoInsideWithMom::
# 137 "data//maps/LittlerootTown/scripts.pory"
	delay 15
# 138 "data//maps/LittlerootTown/scripts.pory"
	playse SE_LEDGE
# 139 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, LittlerootTown_Movement_PlayerStepOffTruck
# 140 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 141 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8004, VAR_0x8005
# 142 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 143 "data//maps/LittlerootTown/scripts.pory"
	addobject LOCALID_LITTLEROOT_MOM
# 144 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitHouse
# 145 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 146 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8004, VAR_0x8005
# 147 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 148 "data//maps/LittlerootTown/scripts.pory"
	delay 10
# 149 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerAtTruck
# 150 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 151 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_OurNewHomeLetsGoInside, MSGBOX_DEFAULT
# 152 "data//maps/LittlerootTown/scripts.pory"
	closemessage
# 153 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachDoor
# 154 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, LittlerootTown_Movement_PlayerApproachDoor
# 155 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 156 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8004, VAR_0x8005
# 157 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 158 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomEnterHouse
# 159 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, LittlerootTown_Movement_PlayerEnterHouse
# 160 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 161 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MOM_OUTSIDE
# 162 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_LITTLEROOT_INTRO_STATE, 3
# 163 "data//maps/LittlerootTown/scripts.pory"
	hideplayer
# 164 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8004, VAR_0x8005
# 165 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 166 "data//maps/LittlerootTown/scripts.pory"
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_FAT_MAN
# 167 "data//maps/LittlerootTown/scripts.pory"
	clearflag FLAG_HIDE_MAP_NAME_POPUP
# 168 "data//maps/LittlerootTown/scripts.pory"
	return
# 169 "data//maps/LittlerootTown/scripts.pory"

# 170 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomExitHouse:
# 171 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 172 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 173 "data//maps/LittlerootTown/scripts.pory"

# 174 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerAtTruck:
# 175 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 176 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_left
# 177 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 178 "data//maps/LittlerootTown/scripts.pory"

# 179 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachDoor:
# 180 "data//maps/LittlerootTown/scripts.pory"
	delay_16
# 181 "data//maps/LittlerootTown/scripts.pory"
	delay_8
# 182 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 183 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 184 "data//maps/LittlerootTown/scripts.pory"

# 185 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomEnterHouse:
# 186 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 187 "data//maps/LittlerootTown/scripts.pory"
	set_invisible
# 188 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 189 "data//maps/LittlerootTown/scripts.pory"

# 190 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_PlayerApproachDoor:
# 191 "data//maps/LittlerootTown/scripts.pory"
	delay_16
# 192 "data//maps/LittlerootTown/scripts.pory"
	delay_8
# 193 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 194 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 195 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 196 "data//maps/LittlerootTown/scripts.pory"

# 197 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_PlayerEnterHouse:
# 198 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 199 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 200 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 201 "data//maps/LittlerootTown/scripts.pory"

# 202 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_PlayerStepOffTruck:
# 203 "data//maps/LittlerootTown/scripts.pory"
	jump_right
# 204 "data//maps/LittlerootTown/scripts.pory"
	delay_16
# 205 "data//maps/LittlerootTown/scripts.pory"
	delay_16
# 206 "data//maps/LittlerootTown/scripts.pory"
	delay_16
# 207 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 208 "data//maps/LittlerootTown/scripts.pory"

# 209 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_BeginDexUpgradeScene::
# 210 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 211 "data//maps/LittlerootTown/scripts.pory"
	playse SE_PIN
# 212 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_BIRCH, Common_Movement_ExclamationMark
# 213 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 214 "data//maps/LittlerootTown/scripts.pory"
	delay 80
# 215 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_BirchSomethingToShowYouAtLab, MSGBOX_DEFAULT
# 216 "data//maps/LittlerootTown/scripts.pory"
	closemessage
# 217 "data//maps/LittlerootTown/scripts.pory"
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_RIVAL
# 218 "data//maps/LittlerootTown/scripts.pory"
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCH
# 219 "data//maps/LittlerootTown/scripts.pory"
	delay 20
# 220 "data//maps/LittlerootTown/scripts.pory"
	clearflag FLAG_HIDE_MAP_NAME_POPUP
# 221 "data//maps/LittlerootTown/scripts.pory"
	warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 6, 5
# 222 "data//maps/LittlerootTown/scripts.pory"
	waitstate
# 223 "data//maps/LittlerootTown/scripts.pory"
	releaseall
# 224 "data//maps/LittlerootTown/scripts.pory"
	end
# 225 "data//maps/LittlerootTown/scripts.pory"

# 226 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_OnWarp:
# 227 "data//maps/LittlerootTown/scripts.pory"
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, LittlerootTown_EventScript_SetRivalBirchPosForDexUpgrade
# 228 "data//maps/LittlerootTown/scripts.pory"
	.2byte 0
# 229 "data//maps/LittlerootTown/scripts.pory"

# 230 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetRivalBirchPosForDexUpgrade::
# 231 "data//maps/LittlerootTown/scripts.pory"
	addobject LOCALID_LITTLEROOT_BIRCH
# 232 "data//maps/LittlerootTown/scripts.pory"
	addobject LOCALID_LITTLEROOT_RIVAL
# 233 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 234 "data//maps/LittlerootTown/scripts.pory"
	goto_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_SetRivalBirchPosForDexUpgradeMale
# 235 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_SetRivalBirchPosForDexUpgradeFemale
# 236 "data//maps/LittlerootTown/scripts.pory"
	end
# 237 "data//maps/LittlerootTown/scripts.pory"

# 238 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetRivalBirchPosForDexUpgradeMale::
# 239 "data//maps/LittlerootTown/scripts.pory"
	setobjectxy LOCALID_LITTLEROOT_RIVAL, 6, 10
# 240 "data//maps/LittlerootTown/scripts.pory"
	setobjectxy LOCALID_LITTLEROOT_BIRCH, 5, 10
# 241 "data//maps/LittlerootTown/scripts.pory"
	end
# 242 "data//maps/LittlerootTown/scripts.pory"

# 243 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetRivalBirchPosForDexUpgradeFemale::
# 244 "data//maps/LittlerootTown/scripts.pory"
	setobjectxy LOCALID_LITTLEROOT_RIVAL, 13, 10
# 245 "data//maps/LittlerootTown/scripts.pory"
	setobjectxy LOCALID_LITTLEROOT_BIRCH, 14, 10
# 246 "data//maps/LittlerootTown/scripts.pory"
	end
# 247 "data//maps/LittlerootTown/scripts.pory"

# 248 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_FatMan::
# 249 "data//maps/LittlerootTown/scripts.pory"
    createfieldmugshot MUGSHOT_TEST, EMOTE_NORMAL
# 250 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_CanUsePCToStoreItems, MSGBOX_NPC
# 251 "data//maps/LittlerootTown/scripts.pory"
	end
# 252 "data//maps/LittlerootTown/scripts.pory"

# 253 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_Boy::
# 254 "data//maps/LittlerootTown/scripts.pory"
    createfieldmugshot MUGSHOT_TEST, EMOTE_NORMAL
# 255 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_BirchSpendsDaysInLab, MSGBOX_NPC
# 256 "data//maps/LittlerootTown/scripts.pory"
	end
# 257 "data//maps/LittlerootTown/scripts.pory"

# 258 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_Huh::
# 259 "data//maps/LittlerootTown/scripts.pory"
	lock
# 260 "data//maps/LittlerootTown/scripts.pory"
	faceplayer
# 261 "data//maps/LittlerootTown/scripts.pory"
	createfieldmugshot MUGSHOT_TEST, EMOTE_NORMAL
# 262 "data//maps/LittlerootTown/scripts.pory"
	setspeaker Speaker_Jeremy
# 263 "data//maps/LittlerootTown/scripts.pory"
	questmenu QUEST_MENU_CHECK_COMPLETE, QUEST_1
# 264 "data//maps/LittlerootTown/scripts.pory"
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_EventScript_Jeremy_Done
# 265 "data//maps/LittlerootTown/scripts.pory"
	questmenu QUEST_MENU_CHECK_ACTIVE, QUEST_1
# 266 "data//maps/LittlerootTown/scripts.pory"
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_EventScript_Jeremy_Active
# 267 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_Jeremy_GiveQuest, MSGBOX_DEFAULT
# 268 "data//maps/LittlerootTown/scripts.pory"
	questmenu QUEST_MENU_SET_ACTIVE, QUEST_1
# 269 "data//maps/LittlerootTown/scripts.pory"
	removefieldmugshot
# 270 "data//maps/LittlerootTown/scripts.pory"
	release
# 271 "data//maps/LittlerootTown/scripts.pory"
	end
# 272 "data//maps/LittlerootTown/scripts.pory"

# 273 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_Jeremy_Active::
# 274 "data//maps/LittlerootTown/scripts.pory"
	getcaughtmon SPECIES_ZIGZAGOON
# 275 "data//maps/LittlerootTown/scripts.pory"
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_EventScript_Jeremy_Reward
# 276 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_Jeremy_StillLooking, MSGBOX_DEFAULT
# 277 "data//maps/LittlerootTown/scripts.pory"
	removefieldmugshot
# 278 "data//maps/LittlerootTown/scripts.pory"
	release
# 279 "data//maps/LittlerootTown/scripts.pory"
	end
# 280 "data//maps/LittlerootTown/scripts.pory"

# 281 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_Jeremy_Reward::
# 282 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_Jeremy_GiveReward, MSGBOX_DEFAULT
# 283 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_CAN_SEE_WILD_NATURE
# 284 "data//maps/LittlerootTown/scripts.pory"
	questmenu QUEST_MENU_COMPLETE_QUEST, QUEST_1
# 285 "data//maps/LittlerootTown/scripts.pory"
	removefieldmugshot
# 286 "data//maps/LittlerootTown/scripts.pory"
	release
# 287 "data//maps/LittlerootTown/scripts.pory"
	end
# 288 "data//maps/LittlerootTown/scripts.pory"

# 289 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_Jeremy_Done::
# 290 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_Jeremy_Complete, MSGBOX_DEFAULT
# 291 "data//maps/LittlerootTown/scripts.pory"
	removefieldmugshot
# 292 "data//maps/LittlerootTown/scripts.pory"
	release
# 293 "data//maps/LittlerootTown/scripts.pory"
	end
# 294 "data//maps/LittlerootTown/scripts.pory"

# 295 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_Twin::
# 296 "data//maps/LittlerootTown/scripts.pory"
	lock
# 297 "data//maps/LittlerootTown/scripts.pory"
	faceplayer
# 298 "data//maps/LittlerootTown/scripts.pory"
	createfieldmugshot MUGSHOT_TEST, EMOTE_NORMAL
# 299 "data//maps/LittlerootTown/scripts.pory"
	goto_if_set FLAG_ADVENTURE_STARTED, LittlerootTown_EventScript_GoodLuck
# 300 "data//maps/LittlerootTown/scripts.pory"
	goto_if_set FLAG_RESCUED_BIRCH, LittlerootTown_EventScript_YouSavedBirch
# 301 "data//maps/LittlerootTown/scripts.pory"
	goto_if_ne VAR_LITTLEROOT_TOWN_STATE, 0, LittlerootTown_EventScript_GoSaveBirch
# 302 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_IfYouGoInGrassPokemonWillJumpOut, MSGBOX_DEFAULT
# 303 "data//maps/LittlerootTown/scripts.pory"
	release
# 304 "data//maps/LittlerootTown/scripts.pory"
	end
# 305 "data//maps/LittlerootTown/scripts.pory"

# 306 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GoSaveBirch::
# 307 "data//maps/LittlerootTown/scripts.pory"
	special GetPlayerBigGuyGirlString
# 308 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_CanYouGoSeeWhatsHappening, MSGBOX_DEFAULT
# 309 "data//maps/LittlerootTown/scripts.pory"
	closemessage
# 310 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_TWIN, Common_Movement_FaceOriginalDirection
# 311 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 312 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_LITTLEROOT_TOWN_STATE, 2
# 313 "data//maps/LittlerootTown/scripts.pory"
	release
# 314 "data//maps/LittlerootTown/scripts.pory"
	end
# 315 "data//maps/LittlerootTown/scripts.pory"

# 316 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_YouSavedBirch::
# 317 "data//maps/LittlerootTown/scripts.pory"
	special GetPlayerBigGuyGirlString
# 318 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_YouSavedBirch, MSGBOX_DEFAULT
# 319 "data//maps/LittlerootTown/scripts.pory"
	release
# 320 "data//maps/LittlerootTown/scripts.pory"
	end
# 321 "data//maps/LittlerootTown/scripts.pory"

# 322 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GoodLuck::
# 323 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_GoodLuckCatchingPokemon, MSGBOX_DEFAULT
# 324 "data//maps/LittlerootTown/scripts.pory"
	release
# 325 "data//maps/LittlerootTown/scripts.pory"
	end
# 326 "data//maps/LittlerootTown/scripts.pory"

# 327 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_NeedPokemonTriggerLeft::
# 328 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 329 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_TWIN, LittlerootTown_Movement_TwinApproachPlayerLeft
# 330 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 331 "data//maps/LittlerootTown/scripts.pory"
	call LittlerootTown_EventScript_DangerousWithoutPokemon
# 332 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_TWIN, LittlerootTown_Movement_TwinReturnLeft
# 333 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 334 "data//maps/LittlerootTown/scripts.pory"
	releaseall
# 335 "data//maps/LittlerootTown/scripts.pory"
	end
# 336 "data//maps/LittlerootTown/scripts.pory"

# 337 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_DangerousWithoutPokemon::
# 338 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_IfYouGoInGrassPokemonWillJumpOut, MSGBOX_DEFAULT
# 339 "data//maps/LittlerootTown/scripts.pory"
	closemessage
# 340 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_TWIN, LittlerootTown_Movement_TwinPushPlayerFromRoute
# 341 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, LittlerootTown_Movement_PushPlayerBackFromRoute
# 342 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 343 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_DangerousIfYouDontHavePokemon, MSGBOX_DEFAULT
# 344 "data//maps/LittlerootTown/scripts.pory"
	closemessage
# 345 "data//maps/LittlerootTown/scripts.pory"
	return
# 346 "data//maps/LittlerootTown/scripts.pory"

# 347 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_TwinApproachPlayerLeft:
# 348 "data//maps/LittlerootTown/scripts.pory"
	face_right
# 349 "data//maps/LittlerootTown/scripts.pory"
	delay_8
# 350 "data//maps/LittlerootTown/scripts.pory"
	disable_jump_landing_ground_effect
# 351 "data//maps/LittlerootTown/scripts.pory"
	jump_in_place_right
# 352 "data//maps/LittlerootTown/scripts.pory"
	delay_8
# 353 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_right
# 354 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_right
# 355 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_right
# 356 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_right
# 357 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_up
# 358 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_up
# 359 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_left
# 360 "data//maps/LittlerootTown/scripts.pory"
	face_down
# 361 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 362 "data//maps/LittlerootTown/scripts.pory"

# 363 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_TwinPushPlayerFromRoute:
# 364 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 365 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 366 "data//maps/LittlerootTown/scripts.pory"

# 367 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_TwinReturnLeft:
# 368 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 369 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 370 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 371 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 372 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 373 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 374 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 375 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 376 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_down
# 377 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 378 "data//maps/LittlerootTown/scripts.pory"

# 379 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_PushPlayerBackFromRoute:
# 380 "data//maps/LittlerootTown/scripts.pory"
	lock_facing_direction
# 381 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 382 "data//maps/LittlerootTown/scripts.pory"
	unlock_facing_direction
# 383 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 384 "data//maps/LittlerootTown/scripts.pory"

# 385 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_NeedPokemonTriggerRight::
# 386 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 387 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_TWIN, LittlerootTown_Movement_TwinApproachPlayerRight
# 388 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 389 "data//maps/LittlerootTown/scripts.pory"
	call LittlerootTown_EventScript_DangerousWithoutPokemon
# 390 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_TWIN, LittlerootTown_Movement_TwinReturnRight
# 391 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 392 "data//maps/LittlerootTown/scripts.pory"
	releaseall
# 393 "data//maps/LittlerootTown/scripts.pory"
	end
# 394 "data//maps/LittlerootTown/scripts.pory"

# 395 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_TwinApproachPlayerRight:
# 396 "data//maps/LittlerootTown/scripts.pory"
	face_right
# 397 "data//maps/LittlerootTown/scripts.pory"
	delay_8
# 398 "data//maps/LittlerootTown/scripts.pory"
	disable_jump_landing_ground_effect
# 399 "data//maps/LittlerootTown/scripts.pory"
	jump_in_place_right
# 400 "data//maps/LittlerootTown/scripts.pory"
	delay_8
# 401 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_right
# 402 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_right
# 403 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_right
# 404 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_up
# 405 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_up
# 406 "data//maps/LittlerootTown/scripts.pory"
	walk_fast_right
# 407 "data//maps/LittlerootTown/scripts.pory"
	face_down
# 408 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 409 "data//maps/LittlerootTown/scripts.pory"

# 410 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_TwinReturnRight:
# 411 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 412 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 413 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 414 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 415 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 416 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_down
# 417 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 418 "data//maps/LittlerootTown/scripts.pory"

# 419 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GoSaveBirchTrigger::
# 420 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 421 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_TWIN, Common_Movement_WalkInPlaceFasterRight
# 422 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 423 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
# 424 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 425 "data//maps/LittlerootTown/scripts.pory"
	special GetPlayerBigGuyGirlString
# 426 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_CanYouGoSeeWhatsHappening, MSGBOX_DEFAULT
# 427 "data//maps/LittlerootTown/scripts.pory"
	closemessage
# 428 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_TWIN, Common_Movement_FaceOriginalDirection
# 429 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 430 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_LITTLEROOT_TOWN_STATE, 2
# 431 "data//maps/LittlerootTown/scripts.pory"
	releaseall
# 432 "data//maps/LittlerootTown/scripts.pory"
	end
# 433 "data//maps/LittlerootTown/scripts.pory"

# 434 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_TownSign::
# 435 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_TownSign, MSGBOX_SIGN
# 436 "data//maps/LittlerootTown/scripts.pory"
    setweather WEATHER_LEAVES
# 437 "data//maps/LittlerootTown/scripts.pory"
    doweather
# 438 "data//maps/LittlerootTown/scripts.pory"
	end
# 439 "data//maps/LittlerootTown/scripts.pory"

# 440 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_BirchsLabSign::
# 441 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_ProfBirchsLab, MSGBOX_SIGN
# 442 "data//maps/LittlerootTown/scripts.pory"
	end
# 443 "data//maps/LittlerootTown/scripts.pory"

# 444 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_BrendansHouseSign::
# 445 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 446 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 447 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_PlayersHouseSignMale
# 448 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_BirchsHouseSignFemale
# 449 "data//maps/LittlerootTown/scripts.pory"
	releaseall
# 450 "data//maps/LittlerootTown/scripts.pory"
	end
# 451 "data//maps/LittlerootTown/scripts.pory"

# 452 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_PlayersHouseSignMale::
# 453 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_PlayersHouse, MSGBOX_DEFAULT
# 454 "data//maps/LittlerootTown/scripts.pory"
	return
# 455 "data//maps/LittlerootTown/scripts.pory"

# 456 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_BirchsHouseSignFemale::
# 457 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_ProfBirchsHouse, MSGBOX_DEFAULT
# 458 "data//maps/LittlerootTown/scripts.pory"
	return
# 459 "data//maps/LittlerootTown/scripts.pory"

# 460 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MaysHouseSign::
# 461 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 462 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 463 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_BirchsHouseSignMale
# 464 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_PlayersHouseSignFemale
# 465 "data//maps/LittlerootTown/scripts.pory"
	releaseall
# 466 "data//maps/LittlerootTown/scripts.pory"
	end
# 467 "data//maps/LittlerootTown/scripts.pory"

# 468 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_BirchsHouseSignMale::
# 469 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_ProfBirchsHouse, MSGBOX_DEFAULT
# 470 "data//maps/LittlerootTown/scripts.pory"
	return
# 471 "data//maps/LittlerootTown/scripts.pory"

# 472 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_PlayersHouseSignFemale::
# 473 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_PlayersHouse, MSGBOX_DEFAULT
# 474 "data//maps/LittlerootTown/scripts.pory"
	return
# 475 "data//maps/LittlerootTown/scripts.pory"

# 476 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GiveRunningShoesTrigger0::
# 477 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 478 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8008, 0
# 479 "data//maps/LittlerootTown/scripts.pory"
	setobjectxy LOCALID_LITTLEROOT_MOM, 10, 9
# 480 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_GiveRunningShoesTrigger
# 481 "data//maps/LittlerootTown/scripts.pory"
	end
# 482 "data//maps/LittlerootTown/scripts.pory"

# 483 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GiveRunningShoesTrigger1::
# 484 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 485 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8008, 1
# 486 "data//maps/LittlerootTown/scripts.pory"
	setobjectxy LOCALID_LITTLEROOT_MOM, 11, 9
# 487 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_GiveRunningShoesTrigger
# 488 "data//maps/LittlerootTown/scripts.pory"
	end
# 489 "data//maps/LittlerootTown/scripts.pory"

# 490 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GiveRunningShoesTrigger2::
# 491 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 492 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8008, 2
# 493 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_GiveRunningShoesTrigger
# 494 "data//maps/LittlerootTown/scripts.pory"
	end
# 495 "data//maps/LittlerootTown/scripts.pory"

# 496 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GiveRunningShoesTrigger3::
# 497 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 498 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8008, 3
# 499 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_GiveRunningShoesTrigger
# 500 "data//maps/LittlerootTown/scripts.pory"
	end
# 501 "data//maps/LittlerootTown/scripts.pory"

# 502 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GiveRunningShoesTrigger4::
# 503 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 504 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8008, 4
# 505 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_GiveRunningShoesTrigger
# 506 "data//maps/LittlerootTown/scripts.pory"
	end
# 507 "data//maps/LittlerootTown/scripts.pory"

# 508 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GiveRunningShoesTrigger5::
# 509 "data//maps/LittlerootTown/scripts.pory"
	lockall
# 510 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8008, 5
# 511 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_GiveRunningShoesTrigger
# 512 "data//maps/LittlerootTown/scripts.pory"
	end
# 513 "data//maps/LittlerootTown/scripts.pory"

# 514 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GiveRunningShoesTrigger::
# 515 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 516 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_MomNoticePlayerMale
# 517 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_MomNoticePlayerFemale
# 518 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 519 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_SetHomeDoorCoordsMale
# 520 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_SetHomeDoorCoordsFemale
# 521 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_WaitPlayer, MSGBOX_DEFAULT
# 522 "data//maps/LittlerootTown/scripts.pory"
	closemessage
# 523 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 524 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_MomApproachPlayerMale
# 525 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_MomApproachPlayerFemale
# 526 "data//maps/LittlerootTown/scripts.pory"
	call LittlerootTown_EventScript_GiveRunningShoes
# 527 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 528 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_MomReturnHomeMale
# 529 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_MomReturnHomeFemale
# 530 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_SetReceivedRunningShoes
# 531 "data//maps/LittlerootTown/scripts.pory"
	end
# 532 "data//maps/LittlerootTown/scripts.pory"

# 533 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetHomeDoorCoordsMale::
# 534 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8009, 5
# 535 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x800A, 8
# 536 "data//maps/LittlerootTown/scripts.pory"
	return
# 537 "data//maps/LittlerootTown/scripts.pory"

# 538 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetHomeDoorCoordsFemale::
# 539 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x8009, 14
# 540 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_0x800A, 8
# 541 "data//maps/LittlerootTown/scripts.pory"
	return
# 542 "data//maps/LittlerootTown/scripts.pory"

# 543 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomNoticePlayerMale::
# 544 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, Common_Movement_WalkInPlaceFasterRight
# 545 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 546 "data//maps/LittlerootTown/scripts.pory"
	return
# 547 "data//maps/LittlerootTown/scripts.pory"

# 548 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomNoticePlayerFemale::
# 549 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, Common_Movement_WalkInPlaceFasterLeft
# 550 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 551 "data//maps/LittlerootTown/scripts.pory"
	return
# 552 "data//maps/LittlerootTown/scripts.pory"

# 553 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerMale::
# 554 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 0, LittlerootTown_EventScript_MomApproachPlayer0
# 555 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 1, LittlerootTown_EventScript_MomApproachPlayer1
# 556 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 2, LittlerootTown_EventScript_MomApproachPlayerMale2
# 557 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 3, LittlerootTown_EventScript_MomApproachPlayerMale3
# 558 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 4, LittlerootTown_EventScript_MomApproachPlayerMale4
# 559 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 5, LittlerootTown_EventScript_MomApproachPlayerMale5
# 560 "data//maps/LittlerootTown/scripts.pory"
	return
# 561 "data//maps/LittlerootTown/scripts.pory"

# 562 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerFemale::
# 563 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 0, LittlerootTown_EventScript_MomApproachPlayer0
# 564 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 1, LittlerootTown_EventScript_MomApproachPlayer1
# 565 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 2, LittlerootTown_EventScript_MomApproachPlayerFemale2
# 566 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 3, LittlerootTown_EventScript_MomApproachPlayerFemale3
# 567 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 4, LittlerootTown_EventScript_MomApproachPlayerFemale4
# 568 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 5, LittlerootTown_EventScript_MomApproachPlayerFemale5
# 569 "data//maps/LittlerootTown/scripts.pory"
	return
# 570 "data//maps/LittlerootTown/scripts.pory"

# 571 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayer0::
# 572 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
# 573 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 574 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayer0
# 575 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 576 "data//maps/LittlerootTown/scripts.pory"
	return
# 577 "data//maps/LittlerootTown/scripts.pory"

# 578 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayer1::
# 579 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
# 580 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 581 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayer1
# 582 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 583 "data//maps/LittlerootTown/scripts.pory"
	return
# 584 "data//maps/LittlerootTown/scripts.pory"

# 585 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerMale2::
# 586 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
# 587 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 588 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerMale2
# 589 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 590 "data//maps/LittlerootTown/scripts.pory"
	return
# 591 "data//maps/LittlerootTown/scripts.pory"

# 592 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerMale3::
# 593 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
# 594 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 595 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerMale3
# 596 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 597 "data//maps/LittlerootTown/scripts.pory"
	return
# 598 "data//maps/LittlerootTown/scripts.pory"

# 599 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerMale4::
# 600 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
# 601 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 602 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerMale4
# 603 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 604 "data//maps/LittlerootTown/scripts.pory"
	return
# 605 "data//maps/LittlerootTown/scripts.pory"

# 606 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerMale5::
# 607 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
# 608 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 609 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerMale5
# 610 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 611 "data//maps/LittlerootTown/scripts.pory"
	return
# 612 "data//maps/LittlerootTown/scripts.pory"

# 613 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerFemale2::
# 614 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
# 615 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 616 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerFemale2
# 617 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 618 "data//maps/LittlerootTown/scripts.pory"
	return
# 619 "data//maps/LittlerootTown/scripts.pory"

# 620 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerFemale3::
# 621 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
# 622 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 623 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerFemale3
# 624 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 625 "data//maps/LittlerootTown/scripts.pory"
	return
# 626 "data//maps/LittlerootTown/scripts.pory"

# 627 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerFemale4::
# 628 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
# 629 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 630 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerFemale4
# 631 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 632 "data//maps/LittlerootTown/scripts.pory"
	return
# 633 "data//maps/LittlerootTown/scripts.pory"

# 634 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomApproachPlayerFemale5::
# 635 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
# 636 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 637 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomApproachPlayerFemale5
# 638 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 639 "data//maps/LittlerootTown/scripts.pory"
	return
# 640 "data//maps/LittlerootTown/scripts.pory"

# 641 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeMale::
# 642 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 0, LittlerootTown_EventScript_MomReturnHome0
# 643 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 1, LittlerootTown_EventScript_MomReturnHome1
# 644 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 2, LittlerootTown_EventScript_MomReturnHomeMale2
# 645 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 3, LittlerootTown_EventScript_MomReturnHomeMale3
# 646 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 4, LittlerootTown_EventScript_MomReturnHomeMale4
# 647 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 5, LittlerootTown_EventScript_MomReturnHomeMale5
# 648 "data//maps/LittlerootTown/scripts.pory"
	return
# 649 "data//maps/LittlerootTown/scripts.pory"

# 650 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeFemale::
# 651 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 0, LittlerootTown_EventScript_MomReturnHome0
# 652 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 1, LittlerootTown_EventScript_MomReturnHome1
# 653 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 2, LittlerootTown_EventScript_MomReturnHomeFemale2
# 654 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 3, LittlerootTown_EventScript_MomReturnHomeFemale3
# 655 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 4, LittlerootTown_EventScript_MomReturnHomeFemale4
# 656 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_0x8008, 5, LittlerootTown_EventScript_MomReturnHomeFemale5
# 657 "data//maps/LittlerootTown/scripts.pory"
	return
# 658 "data//maps/LittlerootTown/scripts.pory"

# 659 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHome0::
# 660 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHome0
# 661 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 662 "data//maps/LittlerootTown/scripts.pory"
	return
# 663 "data//maps/LittlerootTown/scripts.pory"

# 664 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHome1::
# 665 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHome1
# 666 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 667 "data//maps/LittlerootTown/scripts.pory"
	return
# 668 "data//maps/LittlerootTown/scripts.pory"

# 669 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeMale2::
# 670 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHomeMale2
# 671 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 672 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 673 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 674 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 675 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 676 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 677 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 678 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 679 "data//maps/LittlerootTown/scripts.pory"
	return
# 680 "data//maps/LittlerootTown/scripts.pory"

# 681 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeMale3::
# 682 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHomeMale3
# 683 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 684 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 685 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 686 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 687 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 688 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 689 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 690 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 691 "data//maps/LittlerootTown/scripts.pory"
	return
# 692 "data//maps/LittlerootTown/scripts.pory"

# 693 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeMale4::
# 694 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHomeMale4
# 695 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 696 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 697 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 698 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 699 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 700 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 701 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 702 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 703 "data//maps/LittlerootTown/scripts.pory"
	return
# 704 "data//maps/LittlerootTown/scripts.pory"

# 705 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeMale5::
# 706 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHomeMale5
# 707 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 708 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 709 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 710 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 711 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 712 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 713 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 714 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 715 "data//maps/LittlerootTown/scripts.pory"
	return
# 716 "data//maps/LittlerootTown/scripts.pory"

# 717 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeFemale2::
# 718 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHomeFemale2
# 719 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 720 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 721 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 722 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 723 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 724 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 725 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 726 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 727 "data//maps/LittlerootTown/scripts.pory"
	return
# 728 "data//maps/LittlerootTown/scripts.pory"

# 729 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeFemale3::
# 730 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHomeFemale3
# 731 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 732 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 733 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 734 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 735 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 736 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 737 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 738 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 739 "data//maps/LittlerootTown/scripts.pory"
	return
# 740 "data//maps/LittlerootTown/scripts.pory"

# 741 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeFemale4::
# 742 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHomeFemale4
# 743 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 744 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 745 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 746 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 747 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 748 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 749 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 750 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 751 "data//maps/LittlerootTown/scripts.pory"
	return
# 752 "data//maps/LittlerootTown/scripts.pory"

# 753 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_MomReturnHomeFemale5::
# 754 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomReturnHomeFemale5
# 755 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 756 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 757 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 758 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 759 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 760 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 761 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 762 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 763 "data//maps/LittlerootTown/scripts.pory"
	return
# 764 "data//maps/LittlerootTown/scripts.pory"

# 765 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayer0:
# 766 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 767 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 768 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 769 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 770 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 771 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 772 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 773 "data//maps/LittlerootTown/scripts.pory"

# 774 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayer1:
# 775 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 776 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 777 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 778 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 779 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 780 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 781 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 782 "data//maps/LittlerootTown/scripts.pory"

# 783 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerMale2:
# 784 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 785 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 786 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 787 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 788 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 789 "data//maps/LittlerootTown/scripts.pory"

# 790 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerMale3:
# 791 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 792 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 793 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 794 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 795 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 796 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 797 "data//maps/LittlerootTown/scripts.pory"

# 798 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerMale4:
# 799 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 800 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 801 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 802 "data//maps/LittlerootTown/scripts.pory"

# 803 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerMale5:
# 804 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 805 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 806 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 807 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 808 "data//maps/LittlerootTown/scripts.pory"

# 809 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerFemale2:
# 810 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 811 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 812 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 813 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 814 "data//maps/LittlerootTown/scripts.pory"

# 815 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerFemale3:
# 816 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 817 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 818 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 819 "data//maps/LittlerootTown/scripts.pory"

# 820 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerFemale4:
# 821 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 822 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 823 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 824 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 825 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 826 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 827 "data//maps/LittlerootTown/scripts.pory"

# 828 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomApproachPlayerFemale5:
# 829 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 830 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 831 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 832 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 833 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 834 "data//maps/LittlerootTown/scripts.pory"

# 835 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHome0:
# 836 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 837 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 838 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 839 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 840 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 841 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 842 "data//maps/LittlerootTown/scripts.pory"

# 843 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHome1:
# 844 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 845 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 846 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 847 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 848 "data//maps/LittlerootTown/scripts.pory"
	walk_down
# 849 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 850 "data//maps/LittlerootTown/scripts.pory"

# 851 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHomeMale2:
# 852 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 853 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 854 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 855 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 856 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 857 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 858 "data//maps/LittlerootTown/scripts.pory"

# 859 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHomeMale3:
# 860 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 861 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 862 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 863 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 864 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 865 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 866 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 867 "data//maps/LittlerootTown/scripts.pory"

# 868 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHomeMale4:
# 869 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 870 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 871 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 872 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 873 "data//maps/LittlerootTown/scripts.pory"

# 874 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHomeMale5:
# 875 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 876 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 877 "data//maps/LittlerootTown/scripts.pory"
	walk_left
# 878 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 879 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 880 "data//maps/LittlerootTown/scripts.pory"

# 881 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHomeFemale2:
# 882 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 883 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 884 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 885 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 886 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 887 "data//maps/LittlerootTown/scripts.pory"

# 888 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHomeFemale3:
# 889 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 890 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 891 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 892 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 893 "data//maps/LittlerootTown/scripts.pory"

# 894 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHomeFemale4:
# 895 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 896 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 897 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 898 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 899 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 900 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 901 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 902 "data//maps/LittlerootTown/scripts.pory"

# 903 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomReturnHomeFemale5:
# 904 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 905 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 906 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 907 "data//maps/LittlerootTown/scripts.pory"
	walk_right
# 908 "data//maps/LittlerootTown/scripts.pory"
	walk_in_place_faster_up
# 909 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 910 "data//maps/LittlerootTown/scripts.pory"

# 911 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Movement_MomExitThroughDoor:
# 912 "data//maps/LittlerootTown/scripts.pory"
	walk_up
# 913 "data//maps/LittlerootTown/scripts.pory"
	step_end
# 914 "data//maps/LittlerootTown/scripts.pory"

# 915 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_Mom::
# 916 "data//maps/LittlerootTown/scripts.pory"
	lock
# 917 "data//maps/LittlerootTown/scripts.pory"
	faceplayer
# 918 "data//maps/LittlerootTown/scripts.pory"
	checkplayergender
# 919 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, MALE, LittlerootTown_EventScript_SetHomeDoorCoordsMale
# 920 "data//maps/LittlerootTown/scripts.pory"
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_SetHomeDoorCoordsFemale
# 921 "data//maps/LittlerootTown/scripts.pory"
	call LittlerootTown_EventScript_GiveRunningShoes
# 922 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, Common_Movement_WalkInPlaceFasterUp
# 923 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 924 "data//maps/LittlerootTown/scripts.pory"
	opendoor VAR_0x8009, VAR_0x800A
# 925 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 926 "data//maps/LittlerootTown/scripts.pory"
	applymovement LOCALID_LITTLEROOT_MOM, LittlerootTown_Movement_MomExitThroughDoor
# 927 "data//maps/LittlerootTown/scripts.pory"
	waitmovement 0
# 928 "data//maps/LittlerootTown/scripts.pory"
	hideobjectat LOCALID_LITTLEROOT_MOM, MAP_LITTLEROOT_TOWN
# 929 "data//maps/LittlerootTown/scripts.pory"
	closedoor VAR_0x8009, VAR_0x800A
# 930 "data//maps/LittlerootTown/scripts.pory"
	waitdooranim
# 931 "data//maps/LittlerootTown/scripts.pory"
	goto LittlerootTown_EventScript_SetReceivedRunningShoes
# 932 "data//maps/LittlerootTown/scripts.pory"
	end
# 933 "data//maps/LittlerootTown/scripts.pory"

# 934 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_SetReceivedRunningShoes::
# 935 "data//maps/LittlerootTown/scripts.pory"
	removeobject LOCALID_LITTLEROOT_MOM
# 936 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_SYS_B_DASH
# 937 "data//maps/LittlerootTown/scripts.pory"
	setvar VAR_LITTLEROOT_TOWN_STATE, 4
# 938 "data//maps/LittlerootTown/scripts.pory"
	release
# 939 "data//maps/LittlerootTown/scripts.pory"
	end
# 940 "data//maps/LittlerootTown/scripts.pory"

# 941 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_EventScript_GiveRunningShoes::
# 942 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_WearTheseRunningShoes, MSGBOX_DEFAULT
# 943 "data//maps/LittlerootTown/scripts.pory"
	playfanfare MUS_OBTAIN_ITEM
# 944 "data//maps/LittlerootTown/scripts.pory"
	message LittlerootTown_Text_SwitchShoesWithRunningShoes
# 945 "data//maps/LittlerootTown/scripts.pory"
	waitfanfare
# 946 "data//maps/LittlerootTown/scripts.pory"
	setflag FLAG_RECEIVED_RUNNING_SHOES
# 947 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_ExplainRunningShoes, MSGBOX_DEFAULT
# 948 "data//maps/LittlerootTown/scripts.pory"
	msgbox LittlerootTown_Text_ComeHomeIfAnythingHappens, MSGBOX_DEFAULT
# 949 "data//maps/LittlerootTown/scripts.pory"
	closemessage
# 950 "data//maps/LittlerootTown/scripts.pory"
	delay 30
# 951 "data//maps/LittlerootTown/scripts.pory"
	return
# 952 "data//maps/LittlerootTown/scripts.pory"

# 953 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_OurNewHomeLetsGoInside:
# 954 "data//maps/LittlerootTown/scripts.pory"
	.string "MOM: {PLAYER}, we're here, honey!\p"
# 955 "data//maps/LittlerootTown/scripts.pory"
	.string "It must be tiring riding with our things\n"
# 956 "data//maps/LittlerootTown/scripts.pory"
	.string "in the moving truck.\p"
# 957 "data//maps/LittlerootTown/scripts.pory"
	.string "Well, this is LITTLEROOT TOWN.\p"
# 958 "data//maps/LittlerootTown/scripts.pory"
	.string "How do you like it?\n"
# 959 "data//maps/LittlerootTown/scripts.pory"
	.string "This is our new home!\p"
# 960 "data//maps/LittlerootTown/scripts.pory"
	.string "It has a quaint feel, but it seems to be\n"
# 961 "data//maps/LittlerootTown/scripts.pory"
	.string "an easy place to live, don't you think?\p"
# 962 "data//maps/LittlerootTown/scripts.pory"
	.string "And, you get your own room, {PLAYER}!\n"
# 963 "data//maps/LittlerootTown/scripts.pory"
	.string "Let's go inside.$"
# 964 "data//maps/LittlerootTown/scripts.pory"

# 965 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_WaitPlayer:
# 966 "data//maps/LittlerootTown/scripts.pory"
	.string "MOM: Wait, {PLAYER}!$"
# 967 "data//maps/LittlerootTown/scripts.pory"

# 968 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_WearTheseRunningShoes:
# 969 "data//maps/LittlerootTown/scripts.pory"
	.string "MOM: {PLAYER}! {PLAYER}! Did you\n"
# 970 "data//maps/LittlerootTown/scripts.pory"
	.string "introduce yourself to PROF. BIRCH?\p"
# 971 "data//maps/LittlerootTown/scripts.pory"
	.string "Oh! What an adorable POKéMON!\n"
# 972 "data//maps/LittlerootTown/scripts.pory"
	.string "You got it from PROF. BIRCH. How nice!\p"
# 973 "data//maps/LittlerootTown/scripts.pory"
	.string "You're your father's child, all right.\n"
# 974 "data//maps/LittlerootTown/scripts.pory"
	.string "You look good together with POKéMON!\p"
# 975 "data//maps/LittlerootTown/scripts.pory"
	.string "Here, honey! If you're going out on an\n"
# 976 "data//maps/LittlerootTown/scripts.pory"
	.string "adventure, wear these RUNNING SHOES.\p"
# 977 "data//maps/LittlerootTown/scripts.pory"
	.string "They'll put a zip in your step!$"
# 978 "data//maps/LittlerootTown/scripts.pory"

# 979 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_SwitchShoesWithRunningShoes:
# 980 "data//maps/LittlerootTown/scripts.pory"
	.string "{PLAYER} switched shoes with the\n"
# 981 "data//maps/LittlerootTown/scripts.pory"
	.string "RUNNING SHOES.$"
# 982 "data//maps/LittlerootTown/scripts.pory"

# 983 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_ExplainRunningShoes:
# 984 "data//maps/LittlerootTown/scripts.pory"
	.string "MOM: {PLAYER}, those shoes came with\n"
# 985 "data//maps/LittlerootTown/scripts.pory"
	.string "instructions.\p"
# 986 "data//maps/LittlerootTown/scripts.pory"
	.string "“Press the B Button while wearing these\n"
# 987 "data//maps/LittlerootTown/scripts.pory"
	.string "RUNNING SHOES to run extra-fast!\p"
# 988 "data//maps/LittlerootTown/scripts.pory"
	.string "“Slip on these RUNNING SHOES and race\n"
# 989 "data//maps/LittlerootTown/scripts.pory"
	.string "in the great outdoors!”$"
# 990 "data//maps/LittlerootTown/scripts.pory"

# 991 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_ComeHomeIfAnythingHappens:
# 992 "data//maps/LittlerootTown/scripts.pory"
	.string "… … … … … … … …\n"
# 993 "data//maps/LittlerootTown/scripts.pory"
	.string "… … … … … … … …\p"
# 994 "data//maps/LittlerootTown/scripts.pory"
	.string "To think that you have your very own\n"
# 995 "data//maps/LittlerootTown/scripts.pory"
	.string "POKéMON now…\l"
# 996 "data//maps/LittlerootTown/scripts.pory"
	.string "Your father will be overjoyed.\p"
# 997 "data//maps/LittlerootTown/scripts.pory"
	.string "…But please be careful.\n"
# 998 "data//maps/LittlerootTown/scripts.pory"
	.string "If anything happens, you can come home.\p"
# 999 "data//maps/LittlerootTown/scripts.pory"
	.string "Go on, go get them, honey!$"
# 1000 "data//maps/LittlerootTown/scripts.pory"

# 1001 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_CanUsePCToStoreItems:
# 1002 "data//maps/LittlerootTown/scripts.pory"
	.string "If you use a PC, you can store items\n"
# 1003 "data//maps/LittlerootTown/scripts.pory"
	.string "and POKéMON.\p"
# 1004 "data//maps/LittlerootTown/scripts.pory"
	.string "The power of science is staggering!$"
# 1005 "data//maps/LittlerootTown/scripts.pory"

# 1006 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_BirchSpendsDaysInLab:
# 1007 "data//maps/LittlerootTown/scripts.pory"
	.string "PROF. BIRCH spends days in his LAB\n"
# 1008 "data//maps/LittlerootTown/scripts.pory"
	.string "studying, then he'll suddenly go out in\l"
# 1009 "data//maps/LittlerootTown/scripts.pory"
	.string "the wild to do more research…\p"
# 1010 "data//maps/LittlerootTown/scripts.pory"
	.string "When does PROF. BIRCH spend time\n"
# 1011 "data//maps/LittlerootTown/scripts.pory"
	.string "at home?$"
# 1012 "data//maps/LittlerootTown/scripts.pory"

# 1013 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_IfYouGoInGrassPokemonWillJumpOut:
# 1014 "data//maps/LittlerootTown/scripts.pory"
	.string "Um, um, um!\p"
# 1015 "data//maps/LittlerootTown/scripts.pory"
	.string "If you go outside and go in the grass,\n"
# 1016 "data//maps/LittlerootTown/scripts.pory"
	.string "wild POKéMON will jump out!$"
# 1017 "data//maps/LittlerootTown/scripts.pory"

# 1018 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_DangerousIfYouDontHavePokemon:
# 1019 "data//maps/LittlerootTown/scripts.pory"
	.string "It's dangerous if you don't have\n"
# 1020 "data//maps/LittlerootTown/scripts.pory"
	.string "your own POKéMON.$"
# 1021 "data//maps/LittlerootTown/scripts.pory"

# 1022 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_CanYouGoSeeWhatsHappening:
# 1023 "data//maps/LittlerootTown/scripts.pory"
	.string "Um, hi!\p"
# 1024 "data//maps/LittlerootTown/scripts.pory"
	.string "There are scary POKéMON outside!\n"
# 1025 "data//maps/LittlerootTown/scripts.pory"
	.string "I can hear their cries!\p"
# 1026 "data//maps/LittlerootTown/scripts.pory"
	.string "I want to go see what's going on,\n"
# 1027 "data//maps/LittlerootTown/scripts.pory"
	.string "but I don't have any POKéMON…\p"
# 1028 "data//maps/LittlerootTown/scripts.pory"
	.string "Can you go see what's happening\n"
# 1029 "data//maps/LittlerootTown/scripts.pory"
	.string "for me?$"
# 1030 "data//maps/LittlerootTown/scripts.pory"

# 1031 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_YouSavedBirch:
# 1032 "data//maps/LittlerootTown/scripts.pory"
	.string "You saved PROF. BIRCH!\n"
# 1033 "data//maps/LittlerootTown/scripts.pory"
	.string "I'm so glad!$"
# 1034 "data//maps/LittlerootTown/scripts.pory"

# 1035 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_GoodLuckCatchingPokemon:
# 1036 "data//maps/LittlerootTown/scripts.pory"
	.string "Are you going to catch POKéMON?\n"
# 1037 "data//maps/LittlerootTown/scripts.pory"
	.string "Good luck!$"
# 1038 "data//maps/LittlerootTown/scripts.pory"

# 1039 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_TownSign:
# 1040 "data//maps/LittlerootTown/scripts.pory"
	.string "LITTLEROOT TOWN\n"
# 1041 "data//maps/LittlerootTown/scripts.pory"
	.string "“A weenis that can't be shaded any hue.”$"
# 1042 "data//maps/LittlerootTown/scripts.pory"

# 1043 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_ProfBirchsLab:
# 1044 "data//maps/LittlerootTown/scripts.pory"
	.string "PROF. BIRCH'S POKéMON LAB$"
# 1045 "data//maps/LittlerootTown/scripts.pory"

# 1046 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_PlayersHouse:
# 1047 "data//maps/LittlerootTown/scripts.pory"
	.string "{PLAYER}'s HOUSE$"
# 1048 "data//maps/LittlerootTown/scripts.pory"

# 1049 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_ProfBirchsHouse:
# 1050 "data//maps/LittlerootTown/scripts.pory"
	.string "PROF. BIRCH'S HOUSE$"
# 1051 "data//maps/LittlerootTown/scripts.pory"

# 1052 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_BirchSomethingToShowYouAtLab:
# 1053 "data//maps/LittlerootTown/scripts.pory"
	.string "PROF. BIRCH: Well, well, {PLAYER}{KUN}!\n"
# 1054 "data//maps/LittlerootTown/scripts.pory"
	.string "That was good work out there!\p"
# 1055 "data//maps/LittlerootTown/scripts.pory"
	.string "I knew there was something special\n"
# 1056 "data//maps/LittlerootTown/scripts.pory"
	.string "about you when I first saw you,\l"
# 1057 "data//maps/LittlerootTown/scripts.pory"
	.string "but I never expected this.\p"
# 1058 "data//maps/LittlerootTown/scripts.pory"
	.string "Oh, yes. Do you still have the POKéDEX\n"
# 1059 "data//maps/LittlerootTown/scripts.pory"
	.string "I gave you?\p"
# 1060 "data//maps/LittlerootTown/scripts.pory"
	.string "I have something to show you.\n"
# 1061 "data//maps/LittlerootTown/scripts.pory"
	.string "Let's go to my LAB.$"
# 1062 "data//maps/LittlerootTown/scripts.pory"

# 1063 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_WhoAmIHowDidIGetHere:
# 1064 "data//maps/LittlerootTown/scripts.pory"
	.string "The Volbeat around here are\n"
# 1065 "data//maps/LittlerootTown/scripts.pory"
	.string "so active at night!\p"
# 1066 "data//maps/LittlerootTown/scripts.pory"
	.string "They're harder to see at evening,\n"
# 1067 "data//maps/LittlerootTown/scripts.pory"
	.string "but they brighten up as time passes.$"
# 1068 "data//maps/LittlerootTown/scripts.pory"

# 1069 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_Jeremy_GiveQuest:
# 1070 "data//maps/LittlerootTown/scripts.pory"
	.string "JEREMY: Evenin'. Name's Jeremy.\p"
# 1071 "data//maps/LittlerootTown/scripts.pory"
	.string "I've been watching the critters\n"
# 1072 "data//maps/LittlerootTown/scripts.pory"
	.string "on Route 101 for years.\p"
# 1073 "data//maps/LittlerootTown/scripts.pory"
	.string "See if you can catch a Zigzagoon\n"
# 1074 "data//maps/LittlerootTown/scripts.pory"
	.string "out there for me.\p"
# 1075 "data//maps/LittlerootTown/scripts.pory"
	.string "I'll teach you how to read\n"
# 1076 "data//maps/LittlerootTown/scripts.pory"
	.string "a wild {PKMN}'s temperament.$"
# 1077 "data//maps/LittlerootTown/scripts.pory"

# 1078 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_Jeremy_StillLooking:
# 1079 "data//maps/LittlerootTown/scripts.pory"
	.string "JEREMY: Still hunting, eh?\p"
# 1080 "data//maps/LittlerootTown/scripts.pory"
	.string "Zigzagoon are common enough\n"
# 1081 "data//maps/LittlerootTown/scripts.pory"
	.string "on Route 101.\p"
# 1082 "data//maps/LittlerootTown/scripts.pory"
	.string "Skittish little things, but\n"
# 1083 "data//maps/LittlerootTown/scripts.pory"
	.string "they're out there.$"
# 1084 "data//maps/LittlerootTown/scripts.pory"

# 1085 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_Jeremy_GiveReward:
# 1086 "data//maps/LittlerootTown/scripts.pory"
	.string "JEREMY: Ha! You found one.\p"
# 1087 "data//maps/LittlerootTown/scripts.pory"
	.string "Zigzagoon are sharp creatures.\n"
# 1088 "data//maps/LittlerootTown/scripts.pory"
	.string "Watch one long enough and\l"
# 1089 "data//maps/LittlerootTown/scripts.pory"
	.string "you start reading its nature.\p"
# 1090 "data//maps/LittlerootTown/scripts.pory"
	.string "I'll show you what to look for.\n"
# 1091 "data//maps/LittlerootTown/scripts.pory"
	.string "From now on, you'll be able to\l"
# 1092 "data//maps/LittlerootTown/scripts.pory"
	.string "sense a wild {PKMN}'s nature.$"
# 1093 "data//maps/LittlerootTown/scripts.pory"

# 1094 "data//maps/LittlerootTown/scripts.pory"
LittlerootTown_Text_Jeremy_Complete:
# 1095 "data//maps/LittlerootTown/scripts.pory"
	.string "JEREMY: You've got a good eye\n"
# 1096 "data//maps/LittlerootTown/scripts.pory"
	.string "for wild {PKMN} now.\p"
# 1097 "data//maps/LittlerootTown/scripts.pory"
	.string "Keep at it.$"
