#ifndef KPNN_H
#define KPNN_H

#include <math.h>


// use net Pnet => 128x16x2 => opS /egS outputs
#define N_INPUTS   (64 * 2)
#define N_HIDDEN   (8)


// net stuff
double OUTPUT_WEIGHTS[N_HIDDEN] = {-258.2940198087987, 221.0213981912549, -256.9943214189924, -238.604954773721, 226.7998532747939, -187.0884363098505, 183.1886782599442, 214.6737098528202, };


double HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {0.2222252891968122, -0.05396995579543051, 0.02260294813783975, -0.1288709247619244, 0.1352155272081567, -0.01340091019561551, -0.4522297251747128, -0.4161950680037007, -21.66792256966112, 125.514072123407, -7.800988275215732, 89.47515852176839, -36.43308096850491, -16.93133366498332, -26.3582827889012, -218.7010286731567, -420.2586818227753, -230.6364774265952, 243.6185949379169, 71.89171338116621, -322.6021215570998, -79.35883493656959, 154.2167789320663, -33.76357689086615, 102.5259321330565, 10.48298500421955, 192.1634167248291, 198.5365738470541, -50.48196191884777, -142.1664346657508, -283.083571925654, -108.8841805530103, -107.457026620862, 26.72334139219228, -264.6202631506969, 363.6962192418973, -267.3632908510628, -163.5016071548504, 12.21037232926498, -149.3793236190987, -154.7851029548982, 20.41158924929186, -107.2939824116047, -120.0879726305056, 107.6862778309424, -210.4250215021661, 55.36026427716141, -332.9481955284687, -119.3419592719631, 56.27577223748544, 62.4916499830307, -26.7737284418191, 58.99939657711599, 66.42303743800538, 40.26627386558324, -36.22591465039021, 0.2333342506239816, -0.03976034072216617, 0.1247930236276207, -0.2602487363667454, -0.01253536414007439, -0.1677237607854063, -0.03160643416066022, 0.4607196566465868, 0.06236240340506771, -0.4318350562508382, -0.4308097953585953, -0.2209523430657351, 0.2157065969499324, -0.2385343149018168, -0.1399788165651163, 0.1459513202523586, 27.78417507845718, -149.9414793757478, 73.1893137308184, -0.6798813120221251, -56.81256286348626, -50.85176918744605, -118.9183120835151, 26.93308010597524, -194.650326973924, -143.768584044052, 42.10043804258988, -111.7891590000486, 111.4001922494732, -55.27652966345046, -52.89140934049265, 78.53012821464823, -47.56798266178951, -80.39594993562706, -441.5334869667312, 93.33684776116968, -106.3809706185086, -14.46905352776121, -93.91980759254163, -193.4530460147309, -295.445758684805, -13.32149433665952, 225.3038586949209, -308.3330645044379, 128.7257905785683, -35.35145869060424, -74.20834676731842, 18.47566984853022, 272.1580621239787, -420.2440748496467, -119.310874908787, -32.40796562003408, -214.6812165039759, 24.51165558918259, -131.1070575769255, -376.1771676381969, -302.1357995436655, 147.0591335204939, 29.39514513944575, 60.38160374327715, -6.074413097333379, -255.7332076681236, -140.415742718763, 175.6771313750347, -0.2497708824229291, 0.1313364583213518, 0.2058396379956229, 0.2677384613862906, -0.08448850064747426, 0.2555872643159643, 0.3476102081349167, -0.2321485158671385, 0.3745250952777104, -0.2934703881821923, 0.335898066328791, -0.2450713891280216, 0.4254701994012437, -0.4501040316885822, 0.4474707375967273, -0.02156337980253775, -143.0041470015616, -2.079486147030955, -167.6850226795406, -129.3966010771148, 216.6799088505091, -110.6629048237205, -196.1695339792961, -109.3410240086871, 146.1845044848799, -112.3577048347768, 193.0725096076544, -77.50431856961839, -212.6341236825279, -32.69634498754201, -100.850388826337, -231.3578898989556, -176.6581100543546, 210.8718696389356, 32.108651910593, 17.66371446731939, -102.6381140199562, -96.70169078883133, -118.3125503623175, 46.99201470181035, 123.9515286365536, -221.7794482620548, -78.47948219168632, -6.609109264621785, -51.87717067482836, 76.07103771696856, -31.71555312429359, -50.00873475029687, -129.593451745926, 29.21370247975547, -83.4945635896118, 16.38743324677705, 102.742977058884, -69.51738455437616, 72.05881839499621, 211.3782842073362, -137.706346676538, 91.05498575761202, 65.04369476951628, 182.3089634183819, 16.54707704596481, -84.68330554146114, 33.43681050572474, -99.95879764510475, -0.2478092963564253, -0.1289099313453351, 0.4645790932535097, -0.2478028669710285, -0.3332385420022712, -0.4253765511910322, -0.3732713427735824, -0.3310375408413995, -0.479434129306783, -0.2438112072384968, -0.4033663402792841, -0.4835957381797935, -0.1668803345723452, 0.002741480945954777, 0.3360319965686798, -0.3300118338456433, 15.99913926762395, -82.44699743770958, 94.72519528376876, -39.26796813632277, -330.3321880943417, -57.19542649147051, -20.12318166110572, 73.77750746373832, -63.80658050127057, -71.15807783489538, 30.29769624480253, -20.94279276918966, -46.52219060382452, -116.5451855449228, 8.433599563878747, 214.657968396426, -29.89514029179657, -44.25063273685331, 41.64071380583656, -88.67761323798835, -32.7981954272364, 181.2579762765204, 123.1824411199333, 2.199707735736536, -272.804504314812, -44.61800515650353, -28.92112763172969, -170.9276397603447, 99.02925771798695, -13.74251789935569, -218.7500276810799, 165.2365057550702, 206.7773347353147, 89.89791693049931, -65.71050900764511, -10.18754759493703, 61.24811094273755, 132.3358166763156, 33.56299746408029, -111.4461450421101, -273.4505575846336, 41.6161584119376, -124.2445083696319, -55.58816372733037, -138.8954501684655, -237.8580016269489, -88.42372602616142, -62.66463655606081, -0.2307293455725207, 0.1716255618592843, -0.2404384411594078, 0.2977872257110603, -0.4209143570256021, -0.1847565368212557, -0.1531303323121417, 0.2427345177823373, -0.03244097276238767, 0.4123563719505241, 0.2395030575522701, -0.3773839277575649, -0.1861249276837916, 0.4062597639422211, 0.3887243680137789, 0.1383990923121567, 132.6493922046787, 20.27684435895074, 113.0528332405377, 44.55004066901928, 79.61279785830348, 174.1516354330964, 139.7124680327504, 267.5948087022303, -79.45975751093108, -146.8826778685304, -51.35710682570897, 141.1821074436974, -188.3450092653919, 38.34472118990712, 47.90426368127854, 154.3713845529626, -55.54876399560191, 161.0589385000301, 44.60529389584197, -25.91493413902962, 214.3125958779041, -223.7121060953955, -128.1932071218568, 101.670399189425, 25.52761666924793, 53.97093664320182, -81.36736666127963, 44.27328644936976, 205.2553269356315, 11.85542923850096, -196.4031420560833, -21.09588981191069, -61.27624981639196, 21.80699714007561, -16.2949784094458, -65.68691371045726, -108.6915466106684, -4.595557637713876, -103.3642870034271, 5.893914064533513, 20.02617967966723, 2.966870040580032, -2.375944951530609, -9.531515076926334, -84.93496608838254, 40.33534785019702, -26.8482788197648, 1.419166974728902, -0.3381336027933907, -0.2242519882154893, 0.3217924902317079, 0.2780344578335222, 0.001474517631099792, 0.3787412163236836, 0.3617615461637087, 0.139557395428213, 0.1751929999679294, -0.431895293729331, -0.4084846544584654, -0.1220954524456036, -0.3368277646772693, -0.3181276115673257, 0.00392557983469477, 0.1650283092935702, -9.719929967397611, 139.6515832735109, -28.03026209767281, 26.29798283692821, -1.417276885551289, -1.836830898023156, -40.58753986700732, -14.35232220208159, -121.0959798750641, 112.3548348365359, -139.377145471424, 47.77024465909849, 39.76092761618305, -10.90532953863044, 132.4087552287531, 40.1023133423096, -2.048467986022357, 18.68251596634239, 22.59299810340882, 40.83030591834992, 109.7946844766358, -249.0550791890647, -99.39390391673317, -36.23333155646703, -5.318009643767135, 36.59161137805904, 44.36723064271109, 77.47381000312816, 224.9584318292701, -72.26613036144761, 184.7873056815133, -221.3713277518503, -113.6872809560882, 155.1185842164845, 134.2680408001803, -76.76644269691128, 36.5799601649539, -231.8123247016242, 161.5047475859806, 147.1809604761854, 15.2483049931545, 194.5920329075382, -0.1663200388961877, 12.83528008902543, -21.44169289502262, -38.60179771993988, 162.4945438952136, 117.4439071652991, 0.2891170805269466, 0.02610602254332323, -0.365181211785032, 0.4060804894687983, 0.40044070356546, 0.1171040672888533, 0.1982368983785794, -0.1571199091417342, -0.3090953085613881, 0.3682978553084181, 0.4484917139394636, 0.4465777044867061, 0.04228439160729031, -0.2808683541514297, -0.19207644215416, 0.05743976475551715, -469.8651395793523, 46.73555255646595, 47.63441946715198, -1.417818576981731, -141.4767422649822, -552.2384355671174, -101.1159196624856, -166.2808851347429, 165.1211879846677, 43.30757839787484, 181.2844377438452, 28.71493983291744, -25.67892699820211, -137.6886544262528, -59.1418229793342, -198.8067849431185, -380.6298873229561, 126.3141569018995, 6.97199231929764, 3.281597656841368, 101.5005117228113, -127.4964936742739, 242.9853878056739, 84.83846544695076, 199.2520741028216, 3.321403735143925, -42.1832462181325, -239.3913530300251, 360.1760456577425, -33.76006224364455, -55.57539519176563, -275.4567737464722, 70.55636427361192, 32.49223968783001, -173.5270643633588, -60.4961409715955, 409.4751139878282, 152.654886838569, 61.61907128810689, -79.15061881302205, 46.18835427561332, -30.97304983248936, 33.54771517832202, 15.95029175427831, 10.43897361279021, 86.2966439533059, 45.26008478534815, 56.44559052483953, -0.03284599004911537, 0.3452762383247149, -0.05539773616725474, 0.4854206563836991, 0.1197853263559683, 0.1178328537465226, 0.1945919937894642, -0.2374077307700215, 0.2395551580654248, -0.1496589643180645, -0.0523163958230598, -0.419590892232764, -0.1859863645797531, -0.3097852961205809, -0.1978482230090761, 0.1880495276712112, -78.90935691469566, -50.16437317631371, -82.01652042076748, 85.26885144478231, -127.918863580569, -81.4315153476757, 8.781256803955552, -14.21311706933444, 71.76886163680456, -12.34715104768413, -23.40257757036136, 184.1749694885206, -29.02735639198984, 7.475655006303906, -22.77272498531653, -22.62770390310626, 95.49678647642581, -292.6194613053126, 193.9266300426426, -92.13728353392275, -54.69524451647882, -87.92358755145479, 130.5928359904719, -173.7939888122932, -127.5947437656436, 141.0648089714594, -45.81854920017737, -68.25955989558477, -315.8977337649271, -202.6033296406304, -93.65375525501261, 482.721287559819, 183.8349699347786, -343.2888717946686, -151.1067636397759, 117.5602617198111, -169.6379383785756, 179.3297574460278, -62.88708800528887, -253.563076288212, -79.79987580184174, 278.939346454538, -120.4434934121137, -20.98728747423361, -172.0017034353477, -35.57328509097098, -88.25307335525795, 180.0618321254987, -0.05144293119732424, -0.4334200224529113, 0.005593119890239584, -0.05256841078054553, -0.4391291648797361, -0.2996456966733773, -0.3141387322983419, -0.094497271158964, -0.1572009090600539, -0.4952999460489024, -0.4358517420179452, 0.1754585828983498, -0.3743093818772162, -0.08915668474005378, -0.1377709687863341, -0.2593235907886753, -180.8142701460829, -61.49271367823371, -75.02742284722932, -147.6264533972052, -33.21885702333604, 77.73510688034949, 141.1504210056523, -36.60904463541533, 11.30938489180828, -135.5046779230055, -78.41554186215174, 85.06256384909554, 150.1587376093679, -120.9686816727827, -153.9758874868047, -256.8062229525412, -99.95919911983783, 172.1682465784184, -72.12811184815787, -140.3013647011346, 36.69738245236412, -97.9484767170643, -412.087013365912, -208.0477129092715, -83.94144909421189, -89.24241603082935, -69.84005961251763, 227.1093892070456, 124.3622249174557, -11.03186434790138, -257.3360180644832, -35.58574943812307, -98.89296597302894, 120.1309707868804, 44.10903702082267, -73.7418081970054, 188.9290466475653, 122.8731097462034, -57.41806891305909, -102.3927771674223, -30.94067808516881, -33.99274071624196, -169.2112802742925, 201.7248735284972, 19.59454417421411, 187.8096102562779, 78.05573763330247, -50.52293526570563, -0.130243156864421, -0.4235127172076668, -0.2379385054753806, 0.03714775039681595, 0.06143820218808865, -0.3505864459325496, -0.4526622518676623, -0.4489108244650581, -0.3990861279420025, -0.2670136703024683, 0.1664307679359013, -0.03674249422584774, 0.2808639424763918, 0.02244876908252424, -0.4498203261521739, 0.03383140337366208, -19.12988915939862, 68.81043440422879, 141.3574854220915, -18.26108845656189, -6.174908647218306, -2.414124941911901, -9.97375554441863, -49.1694153457844, -123.8359083664648, 184.336401922539, -60.4391359877797, -147.5095439559975, -188.8733835338686, -154.7413481981975, 83.23339702938648, -79.1967874660368, 108.6447616500816, -38.43855339097692, 109.9509005861834, 164.5784974182934, 53.48664404505674, -125.5967926755298, -27.68718162510008, -77.49295604443417, -47.15015191585753, 205.2733100067154, -40.54534659138293, -286.9183922362363, -138.5316040774744, -22.47135814142569, -212.9888441788477, -181.2723012690993, 80.41992736027737, -181.4930566024009, 327.6173140375938, -80.903957418619, 20.65577156580781, -295.1541989717881, -2.154894851321061, -322.0564492366295, 55.51693956428993, -372.4282099345972, 114.909912629545, 72.83232867884558, 189.1244066786983, 59.96561628511845, -226.4523525638947, -26.57097241393371, 0.3446636189914605, 0.2071108872569682, -0.3822042582939399, 0.1339720574365799, -0.3994312821419124, -0.247637759310956, 0.02419436887101939, 0.2270128786224, -0.4466404923920708, 0.3776741860796112, -0.365679018602557, -0.1231944563906614, -0.218666322863971, -0.4245395119881907, 0.3159337755366851, 0.2177162397269701, 98.91964078304346, 37.78349735531362, -174.1778906541501, 20.82519795761451, -192.3158434263879, 266.9632257802445, -24.21210187960339, 124.064798429747, -107.6853580716424, 154.0125030169011, 186.044759185981, -3.588835920694551, 193.8695745692311, -142.2044055217873, -136.8084897675525, -370.778610560146, -169.8822098606689, 65.45788608913922, 144.825630770996, 80.14459843153493, -76.70061533020863, 72.95573957221539, -18.68806233253659, -7.022449664165496, -36.685211047214, 94.92669155067169, 5.665365541452883, -74.45565165640441, -242.384430220769, -55.5373527237865, 287.2490881327469, 48.67666343668972, -137.4938185415229, -105.6263712092188, -76.35651227901724, -76.79220264875752, -52.81151151644089, 160.3236362580545, 181.7411151245829, -11.27227760047972, 59.88842775820742, -15.05039907437492, -105.7700465378016, -13.24900051064322, 37.09293891539114, 67.44625946403305, -103.431193634725, 99.43415280227489, -0.3093706773637658, 0.310936387074616, -0.09111059950250699, 0.4465606012132767, 0.3985458458301359, 0.3427249066730612, 0.2558513873982483, 0.3638294901996057, -0.08911860621958911, 0.4419477171925584, -0.3895531882948955, -0.44550163110043, 0.4439778225235538, -0.4252196265967654, 0.03094999423760453, 0.2430309484447497, -96.26600864513367, -22.68081402832376, 49.77506129196143, 70.75842021085853, 7.343702843059814, -34.50917207472849, 9.370171733791253, 99.90159253329882, -317.6059305985865, 13.17963802339723, 17.14450482670392, -101.3744646387861, -41.04189608433403, 26.83394041581186, -74.8307095520845, 115.4165517739211, -342.7736191964348, -110.1683381190148, -30.2074970804455, 173.4844554841053, -54.11795354006519, 117.9441591445295, 24.59762263235688, 27.70408611285971, 215.4115942584271, -428.4688103626094, -471.0668885113295, -3.893228034365058, -158.815545211709, 59.70711583319657, -16.44570708733864, -289.3970399789594, -150.2095027944383, 150.4873349647925, 45.41600764086105, -320.3160332744578, 255.3543108886595, 250.8939405897856, 50.55065578589789, -189.0034558259717, 188.2384753444245, -309.4618162547553, 74.62808031130297, -295.6484255863617, -20.3242167381524, 147.1340356201436, -185.4424527886966, -20.05064824469919, 0.1560405551716874, 0.140057761054513, 0.456084695624227, 0.4761368008219343, 0.4949237969680335, 0.01274583698843879, 0.2071144402525921, -0.2082624503915489, -0.3201462094765838, -0.007409551417180138, 0.03252912523808382, 0.4217421179272896, 0.3090164516163135, 0.01635164931246624, -0.4233807599746533, 0.4566052877142119, 33.40928743500745, -43.22935058320203, 9.593241766142622, -79.92889109447985, 14.64371018823763, 369.2326623274166, 75.31996187661017, 55.88033219920091, -149.8024997917199, -80.80324164769763, -43.21051959200807, -88.16611308950759, 192.724206059736, -35.60639437390911, 22.51833191566459, -83.49278062589818, -69.06185550781554, 5.535085276332547, -33.29043099615355, -19.60855405542357, 4.140710452787444, 32.22510469661362, 171.4375797857331, -45.90673200803237, -22.33905061519093, 31.40855043452148, 125.5285830699663, 48.2632727744116, 115.7180892876356, 111.4392095081343, 119.7252869445192, -108.8425009858497, -84.38350021525268, -5.659290555546629, 10.29648137532887, 71.24487622610914, 45.47598201231335, 201.1622903744081, 8.543892283614756, 24.37005937764215, 52.09994046699438, 35.19497793639064, 30.81851789907181, 77.73156081804214, 81.62977668016613, -121.3884955201521, 7.629531709830055, 73.09437518058778, -0.1683255497684356, -0.02888484649773915, 0.1876565244457016, -0.4432220882471753, -0.2682056244314674, 0.3779137506512523, 0.4617351642631624, -0.0149359533167146, 0.1007001025605482, 0.2884345342351284, 0.04735016941435177, 0.3398381037823102, 0.1917531446981026, 0.06624157985963003, 0.4945651097197855, -0.1497865494572495, 5.652061543273784, 45.79187748566883, 58.23856288791317, -85.84072183338456, 20.7467632771271, -6.194814433365466, 5.850164634062074, -49.69484855821002, 20.1012578509051, 51.60303084624859, -132.9091841880047, -248.5517831061346, 44.62908717725065, 81.1012183993747, -184.5390302044766, -167.4207009466494, -166.3863592278183, 43.32062318658726, 175.4774536027079, 18.88168182995459, -320.2354986705386, 100.5384036861498, 76.60283791544857, -49.2695209968778, 221.1881869453267, 61.69680200762939, 168.1179675103398, 196.0166638193244, 38.06674508722414, 61.78918317313939, -27.84637862836349, 24.21341737995133, 199.9820638012414, 97.94667211130401, 62.10509348882371, 175.0218899199646, 86.17199407756026, -103.1030775987891, -65.56961015703575, -49.43856512983324, 50.7971889625848, 46.79512684390244, -111.9693056789779, 93.57249245697356, 79.62290993711702, -141.9650173584111, 334.668619938376, 301.9191998034003, -0.3442812388969032, -0.2353112929199409, 0.315974706232536, -0.1667070964661925, 0.1870104538681965, 0.3944236980259529, -0.388432851940595, 0.3977667344258012, 0.02022285969938287, 0.229679496832974, 0.3002761592158471, 0.2122636012324428, -0.1576430451393328, -0.4470009649856952, 0.2780824614586693, -0.006914654051379587, 231.8595941549535, 246.5988707431116, 241.7188855502117, -99.9994932938675, 48.1457152532078, -174.9116106790296, 57.25788523146501, 270.4704595431137, -77.83773689255648, -7.530677476694293, -74.25723336992144, -337.1082622539161, -176.1817721093323, 70.71590305811559, 336.6669684543691, 208.9212731151812, -137.210340925692, -121.611747051247, 195.8086642033477, 143.413931962783, 169.3224013828145, -207.273333135753, 50.17004661367756, 83.25040888883235, -50.14601183320082, 121.3435811504961, 105.5213978254968, 225.6195564024211, 23.40949841563571, 30.29069317471867, 156.6058006172635, 219.9567308580395, 198.5451315422861, 90.45907674056315, -6.183318548932289, 33.70461682511012, 18.29632639680143, -103.4189854055167, 153.6875136715421, -115.5973331272422, -59.34927876658832, 32.58149919997214, -28.223746758001, 87.5409173573663, -6.950094631743886, 46.66181569190875, 180.9137590381941, -18.62992102104328, 0.2539287944109779, -0.07672102776203349, 0.1988311413204443, 0.3739200867125392, -0.2494431602533176, 0.01984449872739824, 0.01413423871348346, 0.3075142040883723, -0.3588659846498007, 0.3666119919468704, 0.2068285232907294, 0.3154225679186278, 0.4971722061732655, -0.4007718115582931, 0.2112483865168171, -0.3086167736019086, -123.6592929146895, 55.98548713878692, 65.49818531744447, -47.80962684092219, -8.985981742211466, -97.46610342960693, 120.1883095293351, 30.88660002208121, 194.8271831856719, -59.42711075072847, 39.99237286613122, -90.99147185769063, -14.32177060120447, -161.5780565156603, -167.3041323406076, -5.489448370977055, 26.06392520946521, 31.51796994571067, -82.19157354291758, 158.2771681009792, 121.8939352548448, -61.95960841488218, -135.8995965021685, -191.2945438909, -114.5739444103857, -351.8694872955457, -291.5910896394378, 71.06495298544932, 147.6567758766231, -240.2136418623049, 388.9796656969657, -172.7535223467169, -104.7150813036967, -64.67484150750234, -58.78224237234583, 16.1336068751511, 66.15157847956426, 212.0740933900982, 47.4532759096657, -68.98340452579856, 324.4134686301747, 314.7254265525344, -133.6177376729121, -67.58715030862487, -67.23504870905086, 148.7536509229205, -290.1585688330168, 71.70138957960644, 0.3198636569175234, 0.1407533230449787, 0.06100956050726097, -0.3537845052097852, -0.1830983290835742, -0.2861867420311024, 0.1137013484787668, 0.3596620107347435, };



double sigmoid(double x){
    return 1 / (1 + exp(-1 * x));
}

#endif