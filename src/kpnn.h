#ifndef KPNN_H
#define KPNN_H

#include <math.h>


// use net Pnet => 128x16x2 => opS /egS outputs
#define N_INPUTS   (64 * 2)
#define N_HIDDEN   (8)


double OUTPUT_BIAS = 0.1545956572312633;
double HIDDEN_BIAS[N_HIDDEN] = {-0.5018004302684295, 0.3444357531029529, 0.2089581773431271, 0.001333516206048291, 0.4885292874838058, -0.4760969409673629, 0.07594633907239991, 0.1328940328907565, };
double OUTPUT_WEIGHTS[N_HIDDEN] = {0.441988384729463, 0.3716996830024714, 0.4081354752682791, 0.01027696826906173, -0.4542675524718073, 0.1872969863289799, 0.2720162200300827, 0.1999433456626205, };


double HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {0.2281528887004372, 0.08403323385120987, 0.2437570773734511, 0.1971495382940162, 0.03873036174975819, 0.04883303518818366, 0.2146090617005755, 0.423646761534571, -0.3229483255561685, -0.4631401279955607, 0.1081704857104687, 0.1264279208083942, -0.4671373368532363, 0.02940563402959761, -0.1542403827252026, 0.4859137574956127, 0.03204850726589207, -0.2286730461912316, 0.3287878181297921, -0.05249837806095723, 0.4970732809250787, -0.1113167522313486, 0.4537495374945296, -0.4114393482851033, -0.135564047608328, 0.1811127073200698, -0.3850056151990555, -0.3438194249233588, -0.2405704047115802, 0.3696508126455063, 0.285669212953521, 0.4829496742167189, -0.04931725097168032, 0.03177801048741614, 0.1878019245698653, 0.4839445217349214, -0.4197855395671086, -0.1032079385162564, 0.4158909827044543, -0.2411534352920634, -0.06612863967385262, 0.02460416298218263, 0.3864380764297745, -0.0343493805857371, -0.4532647115689915, -0.2643989710051513, -0.04987386376306783, 0.077479961296111, 0.007418071260418818, -0.2191954638615755, -0.4726481178813397, -0.001048937668583545, 0.1729936366138533, 0.477932077408935, 0.09496528383249028, -0.4537459336009756, 0.1562021303252327, 0.2214568959183325, -0.3037851540389401, 0.4170404495285081, 0.09093434810216278, 0.4835293586289181, 0.4060316574322207, -0.4555549658627971, 0.01460094773890497, 0.0921724036299495, -0.4633139180779987, 0.09450557250273672, 0.4929222119939151, 0.4525738921726932, 0.3526686853043124, -0.07156670772077833, -0.02329313562858461, 0.2367377511468589, 0.3937812480111079, 0.02393358120406496, 0.4736572417184922, -0.1569326362335356, -0.3999826939097878, -0.01872528080584969, 0.1294141032153282, -0.3724767634824684, 0.4829662428109165, -0.1967329309997652, -0.3937752955766025, 0.07569255569971998, -0.1508502209444112, 0.2616680990332595, -0.1996157957718533, 0.04889708607785317, 0.1815438183166092, 0.3864516922352986, 0.030995176206703, 0.08403595122629576, 0.4314537731973812, -0.458433903223415, -0.3293677537434825, 0.4720835096492665, 0.1300997128737483, -0.3189815625525156, 0.4235880928230297, -0.009325839761476757, 0.09661331421158324, -0.1064468640627853, -0.2750295260066417, -0.0126806479613704, 0.4272016846852595, -0.3040142807068077, 0.3375119952567787, -0.4764465973643383, 0.1700408138916477, -0.04008098298312781, -0.3504543147877191, 0.1586608623954436, 0.2682243729999395, -0.2442000817929275, -0.2601521892999883, -0.3856853147111122, -0.486129733554429, 0.03921577370072189, 0.1630808965596747, 0.1964684136661088, -0.07687962594296771, -0.3085589552338044, -0.2183524415448087, -0.1442354254630559, -0.2655978592883785, -0.04100089312577659, -0.1749051425954816, 0.3718688100911066, 0.1292728672871705, -0.2530009680208755, -0.1379958361564184, -0.2720200800206606, 0.1456232185687978, 0.0893488896961086, 0.2177870692634524, 0.06866981817896964, 0.2881919631936782, 0.05762812195211241, 0.09163714961227344, -0.02531661779708434, -0.482626060247381, 0.2383024349697891, -0.3654815973418413, 0.2926323890893645, 0.4892810281807547, -0.1313531227516697, 0.4146432610750148, -0.4869969968471823, 0.4080790377373287, 0.06871852440421714, 0.2041794962974959, -0.1671388586656694, 0.2613799481886199, 0.4899218068506788, 0.183995098443213, -0.4988083554589562, -0.05210403080038303, -0.4948534469199382, 0.372446522949423, -0.4217177808739686, -0.2413582013980765, -0.2678979458864448, -0.1974564293237606, 0.3997633520211004, 0.3245326932140173, -0.4723333792764596, -0.03046224735645459, 0.1149671039718456, 0.08618877812916186, -0.4393418205214742, -0.4139931792450559, 0.1083484564863751, 0.3006578673477581, -0.2790015121976868, -0.09719826207699828, 0.2914126193955751, 0.09197412302192388, -0.1847536470462522, 0.3062927127874341, -0.002808832074726401, 0.389910281251833, 0.01547813403508083, 0.3274874355771986, 0.1571251422432834, 0.00478540477565742, 0.01275708736514536, 0.1581960467892681, 0.4537619678088287, 0.02312159655761048, 0.03113576096069803, -0.4679886023364908, 0.2804851381948614, 0.2660796399070321, -0.1617592515711482, 0.1834718984009102, 0.09037240808381347, -0.1345999853846617, -0.3457964443814924, -0.2952208691482166, 0.4499676128495323, -0.2854739024405268, -0.2084905916126109, 0.05972336015392464, -0.485612675365158, 0.01119290663413444, 0.4627284603557367, 0.3095668236116568, -0.3966689517202092, -0.219825851327689, 0.1165367107086435, 0.1015896398260682, -0.3317070573766961, -0.3684834019411902, -0.07143786306320107, 0.326956538725766, 0.1389132095782029, 0.4422603817905991, -0.01782924536748758, 0.09148022873489857, -0.03610378252999846, -0.4861840482892299, 0.1207405662673113, -0.2611961951026128, 0.2820673381179322, 0.4547834671017997, 0.4331903509281066, -0.1275908220502015, -0.1734761740914834, -0.4192083429113198, 0.07439724994571198, -0.2251826798941496, -0.2097362927962954, 0.3687825010517478, 0.3343943215943029, -0.191341077356484, -0.1201391663049429, 0.2906008047924353, -0.3845107796609233, -0.02195372384046285, -0.4278100123433017, 0.2352909873082563, -0.4179393591631356, -0.2538724651887793, 0.3617112153027401, 0.003942640880330399, -0.4285966242488325, 0.004707963440896912, -0.04892784615463008, 0.05263916009694297, -0.405813181728969, 0.4152203290328478, 0.06737069718882938, 0.2156770721616582, -0.3401463580504741, -0.1518181262779134, 0.1754080740154759, -0.4120411467328859, 0.2193654583391573, -0.4994609095153683, -0.3297323793311289, -0.2047101313735872, -0.2240073060262983, -0.03353238257497235, -0.3363993889236701, -0.3885278992189991, 0.2795443391551957, 0.04274788553343068, 0.410709228929183, 0.3978858004239699, -0.4756556588609698, 0.484634448652901, 0.1356731807097034, -0.4016176273986239, -0.2740083908791081, 0.01133308731955015, 0.1140601152729569, -0.2026702548232393, -0.4932772773709282, -0.4446170065561084, 0.350143322294745, -0.4034742474326427, 0.4835981072247805, -0.08507224814013818, 0.3245151399777127, -0.3631497676875901, 0.2614364471988394, -0.003526028074300488, -0.2724590410588913, -0.01657115239339136, -0.007007395878412329, -0.1037362670352634, 0.2780842742363533, 0.2749896660874226, 0.3652154721577041, 0.4404829187173682, 0.3864291868945201, 0.1462839390668806, -0.01832435143674313, 0.2948183892717748, 0.04584736081534204, 0.003627561551233483, 0.2781821043556547, -0.3138466465589775, 0.1036575344362731, -0.4932339742374043, 0.1953356640817695, -0.2802495196179648, -0.1979182288848562, 0.2084737957372471, -0.2149935146815739, -0.3480701715909271, 0.3166079268868118, -0.2358750033825053, 0.06850066295289464, 0.1399863789044257, -0.09643669640479458, -0.1667466287811969, -0.3612270950624845, 0.1309604638400304, 0.3173715378238687, 0.1380985223865595, -0.4713747552462736, 0.09677957305534723, -0.0865831666563559, 0.3938193148438909, 0.03846976838003369, -0.2005962118201491, 0.03852889423148732, -0.4800084236376446, -0.4051719684546704, -0.4142972170192222, 0.02376871269409556, 0.3729994203919269, -0.2277351488212044, -0.3693924029974313, 0.3790261720969761, 0.4683421753451366, -0.1491567143373017, -0.3175719198880715, 0.1761217320116128, 0.1361535703056938, -0.1666813185344214, -0.00612245296627621, 0.4014968586492515, 0.4031594457988944, -0.3693363608686315, -0.1953794736942403, -0.2660228338438761, -0.2295690529008004, 0.4327227253808393, -0.4548362240149762, 0.4116955147299302, 0.4559981344330941, 0.1541916126692381, -0.1760818713714129, 0.3546252897662437, -0.3149240485559548, 0.1184949899653539, -0.1049404888689998, -0.2949258339530466, 0.2209139266615914, -0.02202312369296318, 0.2332989995122952, 0.0898860698697735, 0.2421609537843599, 0.3613837934519828, -0.02718268748799458, 0.2234445018798279, -0.2871938852821702, 0.1523895195164442, -0.09999156790889205, 0.3511620118356059, 0.4836219286722884, 0.3936264697953659, 0.2515942932812471, 0.3883366486468989, -0.476164161216544, -0.4448435995470935, -0.3759636221807281, -0.20430991365775, 0.4896789719302575, -0.3228923542531638, -0.2943571437123963, 0.4528267886269962, 0.3269584867297478, 0.02901246004226266, 0.3097939197019646, -0.4847209043729682, -0.3479287847168412, -0.2944056977491852, -0.2758306366926546, 0.3695819404343961, 0.1869803837482971, 0.4574746139824251, -0.03957545599121003, -0.05899032215155964, 0.3214289289096295, 0.4304016760717615, -0.3382190395422663, -0.4625888949718567, 0.08168581279724875, 0.05927590561336987, 0.3893971278876135, 0.06806872034245964, -0.04955746347466091, 0.1402656684802407, -0.04405347274565815, -0.02586141943150189, 0.1952738244485148, 0.08033651067503823, 0.2696882297974885, 0.1857007384933472, 0.2573868119726763, 0.4751348773986414, 0.1383459591756447, 0.08447418139421978, 0.004475971965426585, -0.05223222296054535, 0.09955931716486807, 0.1562353170210332, 0.1538491525437413, 0.3240091062465523, 0.02594543563979192, -0.1591324810936589, 0.2815390982320654, 0.4862374499935597, 0.2814057371336355, 0.1032508244965756, 0.4164389157888549, 0.443194144304806, 0.1408445579103684, -0.001721310709068212, 0.002626309246816342, 0.03003780235039874, -0.433481309193634, 0.4527289181147137, -0.329264760935478, 0.02293258080489164, -0.07322279343997262, 0.3667174600375432, -0.3967391624565885, -0.3035205345617237, 0.0523193965443966, 0.3607439854465164, -0.3281754184645486, -0.309251878787415, -0.05481438038629216, 0.1761821576748892, 0.1389707660483991, -0.4550936501264077, -0.167389050436853, -0.2072122868649718, 0.3688632416859564, 0.3586360504192468, 0.1336085220809369, 0.1502731106985252, 0.3448869885728044, -0.08495095495852586, -0.2464210502767626, 0.2612959549153764, -0.1418311264281542, 0.3944313143345707, -0.2401291571485332, 0.360795030126369, -0.0753645048734769, -0.1735435060411276, 0.3135466531622143, 0.09523155203262862, 0.3493240166805615, -0.2597223019743642, -0.03784181706733693, 0.4527931529838488, -0.0630579177692617, -0.4858198774953379, 0.3132989919761178, 0.108507758583919, -0.295078255020125, -0.2416115537039159, -0.2156127219234943, 0.3440542128287223, -0.1970801403101142, 0.1178124286547187, -0.3631184074473296, -0.3278668559509225, -0.024216003401102, 0.2701934699943863, 0.3223934859923587, -0.1795160773701117, -0.3143322241826761, -0.4241062918084725, -0.4177726384499433, 0.04367713316601565, 0.4700545955658132, -0.1581990670501146, -0.09565226555801032, -0.1052542865849555, 0.1683500713190343, -0.2821348084293416, 0.4903884714472269, 0.01788119725281587, -0.04202096491581771, -0.04746020994292817, -0.02955902392489795, 0.3949501486005961, -0.03352284316603227, -0.2162473475170542, 0.003559023376348891, 0.1715061243956472, 0.04224994897015855, 0.2883500562926522, 0.01550585800572574, 0.3456535957966249, -0.09424796122789758, 0.1522933052164937, -0.4820795161566136, 0.3817900716707995, -0.07731678410308285, 0.3404743959384385, -0.2938568668676834, 0.1077599455039305, 0.4180506803357778, -0.214708304077689, -0.3497294878376195, 0.3825134101408746, 0.1282696932746465, 0.05260627646269169, -0.219074999485096, -0.1988587339753444, 0.2770818766166138, -0.2258019836270974, 0.309818142918339, -0.2703846101891977, 0.2234549438819917, -0.2122304574787953, -0.3680234920699006, -0.3112837147380754, 0.07357578108398106, 0.1285223139002371, 0.3640508623153149, -0.3900579281787654, -0.07928414989052987, -0.1190094660297879, 0.4562765080207266, 0.3243568856052799, -0.4715672265397137, 0.4776989721056236, 0.2074333770195243, -0.04480566724432905, 0.31365736642141, 0.4085580105173622, -0.4374355955275473, 0.2301997128202045, -0.3072553632914008, -0.2883436708730072, 0.1178297647836254, 0.3220250089588756, 0.268891684661651, 0.3994270729323957, -0.3824861096188035, 0.04579567951923885, -0.3281625906689546, 0.4304696991003447, 0.2747230687643416, 0.397470461226496, -0.2847162659737845, 0.4011766309154645, -0.412115583155358, 0.2823718538425732, 0.0320772992130729, 0.4504253314577161, 0.392397792959771, 0.4509477889868188, -0.1715278968548066, 0.3480773274079325, 0.2755703170670059, -0.1411878199508357, 0.3216773456994804, -0.01801708364766885, 0.311835473781375, 0.1357517599294669, -0.1088763669174521, 0.3729389169593057, -0.1337434768910683, 0.08553048554600057, -0.4138220152548499, 0.4841469075839339, -0.09348462267324635, 0.3549957300806578, 0.3842403294459056, 0.02346282830792367, -0.1028494870584553, -0.4439716367203069, -0.04740787435205392, -0.3279911085273722, 0.4531732361958414, 0.168875160359109, -0.4271545684011527, -0.4586178400493175, -0.05034117460974268, 0.1030997707144464, 0.4910716153604081, -0.1547420143437355, 0.05599426066558302, -0.1800865593586534, -0.3073410357173409, -0.1668140384382025, 0.1841080648462571, -0.4890188793697371, 0.3197218018703382, -0.01589422972733301, 0.1487286842313898, -0.2972376385705198, -0.134815016435567, -0.4815856375705175, 0.2907810012910315, -0.04901318250966135, 0.4988587908927577, -0.302415200721976, -0.1985685484057248, 0.3842504008656689, 0.225203420338435, 0.2031842213960246, 0.4393605777204795, -0.3271675296285121, 0.3743627604666416, 0.393479792745404, 0.3412789053599786, 0.4453414323477413, 0.4352106896938884, -0.2092853969054255, 0.05083344483321228, 0.4263101666822611, 0.1375617127109141, -0.392972825045219, -0.2536110890813223, 0.329110411381866, -0.06120877785664458, 0.4252798361355811, 0.3423364557522984, -0.247461814315739, -0.08399376579746309, -0.008685739947802307, -0.04259121722662418, 0.279671550160121, 0.006712449484836513, -0.2524585415387799, -0.2711189945300846, 0.007246107314434641, -0.05530463494267608, 0.03277453972272005, -0.1088760845179626, -0.3339860314773815, -0.2702874700082917, -0.1696747465004411, -0.1605997802451083, -0.3960935432954258, -0.2784285686738244, -0.3191910822423338, -0.4469464291605392, -0.341098738226796, -0.02371309456806688, 0.0982910810348368, -0.418789389986253, -0.3857727282720629, 0.2040674764107369, -0.167611759832083, 0.4417131481795384, -0.3513423655060515, -0.2448690958403478, 0.2820279531410202, -0.1001810765949398, 0.1726469778762958, -0.2234622026354778, 0.354177267850326, -0.0486103019032474, 0.2809975727429602, -0.3939387758117785, 0.1810833816660735, -0.2115586747466557, 0.05018638179500967, -0.2850945160301264, 0.1794825874045823, 0.2144898129501199, -0.05254948988897785, -0.4901143084940056, -0.446817177273868, 0.05358059671175995, -0.2686885211046918, -0.2648030767495227, 0.1054574752361937, -0.1080324376775519, 0.2103625844585998, -0.292435277244176, -0.02265485154867414, 0.3241772292294434, 0.4173444928216489, 0.3082341997922557, 0.2666979766295748, -0.4343692487731433, -0.4352225316386775, 0.05155518024766592, -0.0335448042180132, 0.23732697252991, 0.3277266439179548, -0.1753115768895073, -0.3104519740727041, 0.1106105570730803, -0.06953689063411994, 0.3707992759862911, 0.3998297065961313, 0.4804058205236675, -0.4153375126022546, 0.07892532906025294, 0.1950677634836584, 0.03262760003452111, 0.08847091800338396, 0.2479707976384878, -0.4135965249695592, 0.3221581758274064, 0.4831918415847267, 0.1927631597864379, -0.2856142377393652, 0.1939745128109126, 0.3995263878289421, 0.1917545694308955, 0.01775724855885823, 0.3180239011444652, 0.001008377625822727, -0.2149378387445536, 0.3815097605770961, 0.06537288198517628, 0.3354754582741123, -0.1512295046489499, -0.1988935828711844, 0.1603580814647872, 0.1751591324036745, -0.012928695437642, -0.2223130144289313, -0.395288433623562, -0.1380897453651853, -0.3273300956032926, -0.4175356159802458, -0.05308603068584573, 0.2512008588729625, 0.2811486189731063, 0.4775534453547355, -0.1578209607104308, 0.02802307200081399, -0.4397345083803216, -0.3382959636634648, 0.011947150246713, 0.2580331847823849, -0.1219295864869646, -0.2953219368410221, 0.1582429079012144, -0.4309671222335012, 0.2202672507591548, -0.02504656141731888, 0.07023621609911146, -0.4933454124225981, -0.1434495521911651, -0.365320009116698, 0.3428036909749749, 0.2055308724313653, -0.06354926133693628, 0.006679438290502604, -0.1208002798356117, 0.4277695123701215, 0.2811657426791106, -0.0166683227367086, -0.210112438402191, 0.4554817541295112, 0.06791873256113323, 0.2368771018166454, 0.208036436818533, -0.1510749506177094, 0.2153374294152775, -0.4486401268191632, 0.3765886086645801, 0.2869000519135586, -0.2890565643788637, -0.1139627973350609, 0.04134384429421485, 0.09487595084966961, 0.08524070200944349, -0.3046719778304686, 0.1695778016961559, -0.1880943175338095, 0.1728398628534557, -0.2706574712124792, -0.1857205437723848, -0.4695874942148433, -0.135821823734453, -0.3366957020936562, 0.2321937400145657, 0.3060964910236948, 0.1667504910077396, -0.3911855095761987, 0.2336558542393776, -0.05055339919828537, 0.09709530538820736, -0.4812645548200059, -0.09782156869251699, -0.33921666876822, 0.2608945508396667, -0.3864049713660721, 0.01007834315757995, -0.02148033941821554, -0.3343194135085487, -0.1145087476756959, -0.2399314006392137, -0.1199802949816221, 0.2713470676349084, 0.3007268706668081, 0.4754501422451152, -0.142320496626748, 0.4975103743409348, 0.1412489335725223, 0.1718998965442274, 0.1676923592941733, 0.3765869803682265, 0.4906967301288525, 0.1959042608253212, -0.2522865784132325, -0.3486089137609159, -0.07095907189462292, -0.4452641731804535, 0.3190737202852376, 0.04137731601548256, 0.2895277448881081, -0.2320779034551596, -0.3629546192767819, 0.3142072240888175, 0.1722522269339544, -0.1993678210765905, 0.07576380464051097, 0.2855422719314425, 0.3115983320454128, -0.4462590230821017, 0.4499133782751309, -0.3028297371672651, -0.1856388614193769, -0.1692081185752781, 0.4682354723760087, -0.3857145919856751, -0.1936926521221801, -0.1709684171059317, -0.3880892545444905, 0.4491433820678614, -0.4982658464239426, 0.2802917313404899, 0.3245986368710709, 0.4920402438490224, -0.02470607524983055, -0.4264023747862787, -0.353641414639431, 0.4056469890489985, -0.3740197613708209, 0.4640920031499335, -0.05454987104984712, 0.4157632946730511, -0.271283093253401, 0.07866008384800049, 0.2313559327589348, 0.3967815470163658, 0.3890537735390714, -0.1927075029689545, 0.1886908191755733, 0.1943775854436271, -0.1421505773184588, 0.1363236807981821, 0.3879974490377733, 0.1771272249414612, 0.4658748091944948, 0.36050999663602, 0.2908062570278171, -0.2329219476275046, -0.3130000061500341, 0.3986830788241721, -0.280296366741061, -0.3093191475902877, 0.1804174338790109, -0.4512006940930436, -0.3181946650678589, -0.3466182782120956, -0.3789193157913359, -0.1728445392441212, -0.4394774567054014, -0.2536606335796698, -0.2105627500966949, 0.00667201332127304, -0.3370845130817427, 0.01964113559557179, -0.4101331359288344, 0.3936988319706632, -0.07790275177820716, 0.4736938946757903, -0.2997540192677425, -0.3899043676862047, 0.1691192563945051, 0.06055191371615598, 0.2481976154950437, 0.06195322606725819, -0.2640872422430424, 0.2175274777654146, -0.07671699363255276, -0.474911729293763, 0.4948110407011317, 0.1116830846261811, 0.4251554542519393, -0.2838470452015203, 0.3044385475162306, 0.1018862239980659, -0.23985032263281, 0.4931564740517358, 0.2596015363415642, -0.1187968815372173, -0.1864756911008757, 0.3171037529078489, 0.1287251679599769, 0.1026467722979745, -0.1713110567425394, 0.2890208606833921, -0.3731631602116389, -0.08385873052459117, 0.1802790133210811, 0.04766854993656378, -0.1093543503606982, 0.3838096927851572, 0.1558819080285296, -0.4410952744440054, -0.05762326022314632, -0.09328970047387233, 0.1215731621321354, 0.1783854181843563, -0.3760470515400364, -0.4542502851058446, 0.2029612796138646, -0.3843860820678611, 0.1588341050711531, 0.1266403246661071, -0.1696967811856121, -0.03485903787564911, -0.2719309484743597, 0.09121499760073885, -0.04440008962063964, 0.4897050906096412, 0.471153774090214, 0.2725133533002709, 0.3102797904410342, 0.0983039883888811, -0.1203090299946764, -0.3632656735662677, -0.111958194809015, 0.006147185576216851, 0.05335143932763087, -0.4285215465019092, -0.445299350631097, 0.4436624464316584, 0.455161083934438, 0.2133497464067068, -0.4969072905354701, -0.1006043728443814, -0.379803541526107, 0.1245056081677348, -0.4214395945525913, -0.2561165709725214, 0.1691745179895839, 0.2812008477112316, -0.1402458994683037, -0.1715503173762518, -0.09254285238715937, 0.1892669757789346, 0.2936583738857178, 0.1381122502216614, -0.2191837343003225, -0.2497211962922608, 0.1281102152608405, -0.2476255758077466, -0.4779584096239735, -0.06151094886060325, 0.3501687004555635, -0.09726143714746516, 0.07639048311071453, -0.26040133940327, 0.4073654903613098, -0.3705858133023688, -0.1910224925987917, 0.4623113932443345, -0.429187217531203, -0.2376143464190604, 0.1776975307149891, -0.4285103889719621, 0.168802434675126, 0.296439168297608, 0.1993859574901506, 0.2413127209867824, -0.4629944564665074, -0.1311402376930042, 0.02231436781633735, -0.09817143838445039, 0.1951221881071963, 0.4316016792439285, -0.4102161924195512, -0.01473204922703922, 0.06761512685252119, -0.1293646621297114, 0.2397542206582562, -0.302477200566987, 0.1214281373503953, 0.2631836406411464, 0.1370892299152661, -0.02943551842437794, -0.3346630454136705, -0.2887633106618949, 0.2094675319779047, -0.4284853641542072, -0.1605734967908699, -0.4808725071981886, -0.4659344488596239, -0.08872123648818642, -0.2160514619741829, };





double sigmoid(double x){
    return 1 / (1 + exp(-1 * x));
}

#endif