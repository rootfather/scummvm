/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_TCKEYS
#define MODULE_TCKEYS

namespace Clue {

/*
 * Solve database key conflicts.
 */

typedef enum {
	_Environment_TheClou,
	_Player_Player_1,
	_London_London_1,
	_London_Jail,
	_London_Escape,
	_Evidence_Evidence_1,
	_CompleteLoot_LastLoot,
	_Person_Old_Matt,

	_Location_Holland_Street,
	_Location_Fat_Mans_Pub,
	_Location_Cars_Vans_Office,
	_Location_Tools_Shop,
	_Location_Policestation,
	_Location_Highgate_Out,
	_Location_Hotel,
	_Location_Walrus,
	_Location_Parker,
	_Location_Maloya,
	_Location_Pooly,
	_Location_Nirvana,

	_Ability_Autos,
	_Ability_Locks,
	_Ability_Explosive,
	_Ability_Safes,
	_Ability_Electronic,
	_Ability_Surveillance,
	_Ability_Fight
} KeyConflictE;

/* implemented in data/database.c */
uint32 getKey(KeyConflictE key);

#define Environment_TheClou                 getKey(_Environment_TheClou)
#define Player_Player_1                     getKey(_Player_Player_1)
#define London_London_1                     getKey(_London_London_1)
#define London_Jail                         getKey(_London_Jail)
#define London_Escape                       getKey(_London_Escape)
#define Evidence_Evidence_1                 getKey(_Evidence_Evidence_1)
#define CompleteLoot_LastLoot               getKey(_CompleteLoot_LastLoot)
#define Person_Old_Matt                     getKey(_Person_Old_Matt)

#define Location_Holland_Street             getKey(_Location_Holland_Street)
#define Location_Fat_Mans_Pub               getKey(_Location_Fat_Mans_Pub)
#define Location_Cars_Vans_Office           getKey(_Location_Cars_Vans_Office)
#define Location_Tools_Shop                 getKey(_Location_Tools_Shop)
#define Location_Police_Station             getKey(_Location_Policestation)
#define Location_Highgate_Out               getKey(_Location_Highgate_Out)
#define Location_Hotel                      getKey(_Location_Hotel)
#define Location_Walrus                     getKey(_Location_Walrus)
#define Location_Parker                     getKey(_Location_Parker)
#define Location_Maloya                     getKey(_Location_Maloya)
#define Location_Pooly                      getKey(_Location_Pooly)
#define Location_Nirvana                    getKey(_Location_Nirvana)

#define Ability_Autos                       getKey(_Ability_Autos)
#define Ability_Locks                       getKey(_Ability_Locks)
#define Ability_Explosive                   getKey(_Ability_Explosive)
#define Ability_Safes                       getKey(_Ability_Safes)
#define Ability_Electronic                  getKey(_Ability_Electronic)
#define Ability_Surveillance                getKey(_Ability_Surveillance)
#define Ability_Fight                       getKey(_Ability_Fight)

/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcMain (0, 10000)
 */

/* std */
#define Car_Jeep_1940                          1
#define Car_Jeep_1945                          2
#define Car_Triumph_Roadstar_1947              3
#define Car_Triumph_Roadstar_1949              4
#define Car_Cadillac_Club_1952                 5
#define Car_Opel_Olympia_1953                  6
#define Car_Standard_Vanguard_1950             7
#define Car_Standard_Vanguard_1951             8
#define Car_Standard_Vanguard_1953             9
#define Car_Jaguar_XK_1950                    10
#define Car_Pontiac_Streamliner_1944          11
#define Car_Pontiac_Streamliner_1946          12
#define Car_Pontiac_Streamliner_1949          13
#define Car_Fiat_Topolino_1936                14
#define Car_Fiat_Topolino_1940                15
#define Car_Fiat_Topolino_1942                16
#define Car_Morris_Minor_1948                 17
#define Car_Morris_Minor_1950                 18
#define Car_Morris_Minor_1953                 19

/* profidisk extra */
#define Car_Rover_75_1949                     20
#define Car_Rover_75_1950                     21
#define Car_Rover_75_1952                     22
#define Car_Ford_Model_T__1926                23
#define Car_Bentley_Continental_Typ_R_1952    24
#define Car_Bentley_Continental_Typ_R_1953    25
#define Car_Fiat_634_N_1936                   26
#define Car_Fiat_634_N_1943                   27


/* profidisk extra */
#define Item_Locomotive_front_right          290
#define Item_Locomotive_front_left           291
#define Item_Locomotive_above                292
#define Item_Locomotive_side                 293
#define Item_Locomotive_cabin                294
#define Item_Locomotive_door                 295


/* std */
#define Item_Wooden_door                    9201
#define Item_Steel_door                     9202
#define Item_Painting                       9206
#define Item_Vault                          9207
#define Item_Wall                           9208
#define Item_Wall_corner                    9209
#define Item_Alarm_system_Z3                9210
#define Item_Alarm_system_X3                9211
#define Item_Alarm_system_Top               9212
#define Item_Control_Box                    9213
#define Item_WC                             9214
#define Item_Bar                            9223
#define Item_Fridge                         9224
#define Item_Statue                         9233
#define Item_Window                         9234
#define Item_Cross                          9236
#define Item_Chaplet                        9237
#define Item_Stairway                       9238
#define Item_Wall_gate                      9250
#define Item_Stone_wall                     9253
#define Item_Night_box                      9260
#define Item_Pedestal                       9261
#define Item_Cash_register                  9262
#define Item_Microphone                     9264
#define Item_Picture                        9265
#define Item_Clock                          9266
#define Item_Vase                           9268
#define Item_Loot_bag                       9270
#define Item_Guard                          9272

/* profidisk extra */
#define Item_Confessional                   9273
#define Item_Statue_of_saint                9281
#define Item_Candlestick                    9282
#define Item_Church_cross                   9283
#define Item_Railing                        9284
#define Item_Decorated_column               9285
#define Item_Barrier                        9292
#define Item_African_figure                 9293
#define Item_Batman_figure                  9294
#define Item_Fat_man                        9295
#define Item_Unknown                        9296
#define Item_Jack_the_Ripper_figure         9297
#define Item_King_figure                    9298
#define Item_Guard_figure                   9299
#define Item_Miss_World_1952                9300
#define Item_Mailbag                        9306
#define Item_Ladder                         9307


/* std */
#define Tool_Hand                           9501
#define Tool_Lockpick                       9502
#define Tool_Hammer                         9503
#define Tool_Axe                            9504
#define Tool_Core_drill                     9505
#define Tool_Gloves                         9506
#define Tool_Shoes                          9507
#define Tool_Mask                           9508
#define Tool_Chloroform                     9509
#define Tool_Crowbar                        9510
#define Tool_Angle_grinder                  9511
#define Tool_Drilling_machine               9512
#define Tool_Drilling_winch                 9513
#define Tool_Castle_engraver                9514
#define Tool_Cutting_torch                  9515
#define Tool_Oxyhydrogen_torch              9516
#define Tool_Stethoscope                    9517
#define Tool_Batterie                       9518
#define Tool_Power_generation               9519
#define Tool_Electrical_set                 9520
#define Tool_Protective_suit                9521
#define Tool_Dynamite                       9522
#define Tool_Electric_hammer                9523
#define Tool_Glass_cutter                   9524
#define Tool_Foot                           9525
#define Tool_Rope_ladder                    9526
#define Tool_Radio_equipment                9527
#define Tool_Stabbing_card                  9528


/* std */
#define Loot_Relics                         9618
#define Loot_Crown_jewels                   9633
#define Loot_Abbots_ring                    9634
#define Loot_Trunk                          9635
#define Loot_Deed                           9636


/* std */
#define Person_Matt_Stuvysunt               9801
#define Person_Paul_O_Conner                9802
#define Person_Ken_Addison                  9803
#define Person_Frank_Meier                  9804
#define Person_Herbert_Briggs               9805
#define Person_Tony_Allen                   9806
#define Person_Margret_Allen                9807
#define Person_Mathew_Black                 9808
#define Person_Miguel_Garcia                9809
#define Person_Mike_Seeger                  9810
#define Person_Albert_Liet                  9811
#define Person_Mohammed_Abdula              9812
#define Person_Serge_Fontane                9813
#define Person_Frank_Maloya                 9814
#define Person_Neil_Grey                    9815
#define Person_Mark_Hart                    9816
#define Person_Jim_Danner                   9817
#define Person_Robert_Bull                  9818
#define Person_Thomas_Groul                 9819
#define Person_Helen_Parker                 9820
#define Person_Dan_Stanford                 9821
#define Person_John_Gludo                   9822
#define Person_Miles_Chickenwing            9823
#define Person_Mary_Bolton                  9824
#define Person_Eric_Pooly                   9825
#define Person_Prof_Emil_Schmitt            9826
#define Person_Elisabeth_Schmitt            9827
#define Person_Margrete_Briggs              9828
#define Person_Peter_Brook                  9829
#define Person_Samuel_Rosenblatt            9830
#define Person_Lucas_Grull                  9831
#define Person_Marc_Smith                   9832
#define Person_Jiri_Poulin                  9833
#define Person_Garry_Stevenson              9834
#define Person_Al_Mel                       9835
#define Person_Jack_Motart                  9836
#define Person_Kevin_Smith                  9837
#define Person_Mike_Kahn                    9838
#define Person_Frank_De_Silva               9839
#define Person_Justin_White                 9840
#define Person_John_O_Keef                  9841
#define Person_Luthmilla_Nervesaw           9842
#define Person_Thomas_Smith                 9843
#define Person_Ben_Riggley                  9844
#define Person_Richard_Doil                 9845
#define Person_Sabien_Pardo                 9846
#define Person_Red_Stanson                  9847
#define Person_Pater_James                  9848

/* profidisk extra */
#define Person_Tom_Cooler                   9849
#define Person_Sid_Palmer                   9850
#define Person_Dean_Esters                  9851
#define Person_Melanie_Morgan               9853
#define Person_Prof_Marcus_Green            9852
#define Person_Rosi_Lambert                 9854
#define Person_Rod_Masterson                9855
#define Person_Tina_Olavson                 9856
#define Person_Phil_Ciggy                   9857
#define Person_Pere_Ubu                     9858




/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcBuild (500000, 10000)
 */

/* std */
#define Building_Kiosk                      509000
#define Building_Tante_Emma_Laden           509001
#define Building_Jeweler                    509002
#define Building_Old_Curiosity_Shop         509003
#define Building_Pink_Villa                 509004
#define Building_Retirement_Home            509005
#define Building_Karl_Marx_Grave            509006
#define Building_Villa                      509007
#define Building_Kenwood_House              509008
#define Building_Ham_House                  509009
#define Building_Osterly_Park_House         509010
#define Building_Chiswick_House             509011
#define Building_Suterbys                   509012
#define Building_British_Museum             509013
#define Building_Natural_Museum             509014
#define Building_National_Galery            509015
#define Building_Victoria_and_Albert_Museum 509016
#define Building_Bank_of_England            509017
#define Building_Tower_of_London            509018
#define Building_Starford_Barracks          509019

/* profidisk extra */
#define Building_Baker_Street               509101
#define Building_Madame_Tussaud             509102
#define Building_Westminster_Abbey          509103
#define Building_Downing_Street_10          509104
#define Building_Mail_Train                 509105
#define Building_Tate_Gallery               509106
#define Building_Buckingham_Palace          509107
#define Building_Bulstrode_Chemestry_Ltd    509108

} // End of namespace Clue

#endif
