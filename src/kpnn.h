#ifndef KPNN_H
#define KPNN_H

#include <math.h>


// use net Pnet => 128x16x2 => opS /egS outputs
#define N_INPUTS   (64 * 2)
#define N_HIDDEN   (8)


double OUTPUT_BIAS = -0.2172708841147342;
double HIDDEN_BIAS[N_HIDDEN] = {-0.5471793603474796, 0.2658987582362194, 0.003970656090759949, 0.004899623923027812, 0.4767612614870106, -0.4835391506768173, -0.06682398004729548, 0.1052252783724007, };
double OUTPUT_WEIGHTS[N_HIDDEN] = {0.8901580569348204, 0.3468664066019593, 1.112644618454086, -0.1161422402550358, -0.9274680442478063, -0.09991478310338359, 0.9178740454279123, 0.1503140086812316, };


double HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {0.2281528887004372, 0.08403323385120987, 0.2437570773734511, 0.1971495382940162, 0.03873036174975819, 0.04883303518818366, 0.2146090617005755, 0.423646761534571, -0.3102477887595936, -0.3831664211335826, 0.1188764518672113, 0.1272196425321298, -0.4115188198646905, 0.1984677707639295, -0.2153903163142184, 0.5633456527291488, 0.08575789050287469, -0.2007300100648797, 0.1958205740874704, -0.1018725343194962, 0.6339925679327986, -0.06471458290600782, 0.5067308080859654, -0.5425477464280044, -0.2555116433678329, 0.2628302755338304, -0.5573214425606557, -0.2207409583356548, -0.2314525593152349, 0.372905721413862, 0.2679079724159649, 0.3656178908520223, -0.1010121085517429, 0.05632818300248266, 0.2229453198528427, 0.3518580062491911, -0.4076557375978188, -0.1754737870105202, 0.4182757543562186, -0.2207303715258222, -0.09962648316615877, 0.03458465236080712, 0.4268638219863916, -0.03533762957918225, -0.4537495681140594, -0.2725983753372876, -0.0381883584899204, 0.09614399195888738, 0.0002714459311685078, -0.3214269973914216, -0.4633369606538807, 0.001750455028685817, 0.1633862978523825, 0.4584683054963791, 0.07669588500651962, -0.4366830181142373, 0.1562021303252327, 0.2214568959183325, -0.3037851540389401, 0.4170404495285081, 0.09093434810216278, 0.4835293586289181, 0.4060316574322207, -0.4555549658627971, 0.01460094773890497, 0.0921724036299495, -0.4633139180779987, 0.09450557250273672, 0.4929222119939151, 0.4525738921726932, 0.3526686853043124, -0.07156670772077833, -0.02247622271029906, 0.2271607482286115, 0.3842646883635925, 0.03306236927753159, 0.4792637998967848, -0.1740534497557258, -0.4027075727617669, -0.01606958324796211, 0.1252595278965455, -0.360819390329029, 0.547533913563324, -0.1933271465018971, -0.3859481673711981, 0.0744900466719364, -0.1380320869113008, 0.2522523664664761, -0.1293720795070245, 0.1414196637085573, 0.2352553079049866, 0.3564565091513451, 0.0896892750648919, 0.07114524315207664, 0.4059217661453478, -0.4777857165370333, -0.433903526369729, 0.5381912988334678, 0.02268364616074393, -0.0862049417005602, 0.4870634205370939, 0.01509618082840081, 0.07145212110437424, -0.1943337484348364, -0.2944024934492364, -0.1032887965394245, 0.5408677214930806, -0.3612400449631644, 0.3602354785799405, -0.4246990696315779, -0.03026231931383219, -0.1437474514519232, -0.3205031069030158, 0.1050605021722262, 0.2579909307480475, -0.2315164741048865, -0.2128231246169982, -0.4192475563108727, -0.5262702194730683, 0.1568178719086851, 0.1630808965596747, 0.1964684136661088, -0.07687962594296771, -0.3085589552338044, -0.2183524415448087, -0.1442354254630559, -0.2655978592883785, -0.04100089312577659, -0.1749051425954816, 0.3718688100911066, 0.1292728672871705, -0.2530009680208755, -0.1379958361564184, -0.2720200800206606, 0.1456232185687978, 0.0893488896961086, 0.1867512142529509, 0.09009809325179217, 0.2693601378660937, 0.05640268758659554, 0.1154348157225408, 0.01795058349088809, -0.5637963716603608, 0.2328606316857801, -0.3510240752277575, 0.2819347268465612, 0.4198172272833882, -0.1641980910045783, 0.4514611862046231, -0.4682640879872117, 0.4350885113660129, -0.01098016909898324, 0.1532983846719297, -0.1179247465954705, 0.1900048930715226, 0.4928751908076393, 0.1759294569955492, -0.4926376839637885, -0.06185194402256854, -0.5581416232456851, 0.3660780399550134, -0.4154056854885683, -0.2234841385621307, -0.3119204099901311, -0.2412557040170216, 0.3566607108319358, 0.3280603370391451, -0.4642525770829885, -0.04584350926816563, 0.1237826071918263, 0.103893970727595, -0.4409667707389225, -0.4172789002969296, 0.09797744909584576, 0.3085848845994413, -0.2681463660049673, -0.1008872056492431, 0.2475228249503513, 0.09722626892785823, -0.1843740096101604, 0.3035187459219176, -0.013633391860213, 0.3823044617342445, 0.02425891599445021, 0.3274874355771986, 0.1571251422432834, 0.00478540477565742, 0.01275708736514536, 0.1581960467892681, 0.4537619678088287, 0.02312159655761048, 0.03113576096069803, -0.4679886023364908, 0.2804851381948614, 0.2660796399070321, -0.1617592515711482, 0.1834718984009102, 0.09037240808381347, -0.1345999853846617, -0.3457964443814924, -0.2946581608271601, 0.4452926511221799, -0.2905287295528656, -0.2020462491556836, 0.06290619244586912, -0.4942518781699908, 0.011268009419258, 0.4647780525139366, 0.3083584026223688, -0.3894921007872144, -0.1880391167689646, 0.123132578481526, 0.1137476675273283, -0.3313755889015568, -0.3604797854411287, -0.06761975468961688, 0.3481725747254151, 0.1850498158132195, 0.4611506375522845, -0.03347510609432948, 0.1198288833745292, -0.03964168269203098, -0.4988844292838625, 0.1099128754148637, -0.3332553244406334, 0.2976341093952747, 0.3879194617093725, 0.5021322466721735, -0.1068954772196061, -0.1571266454287113, -0.4384248910001, 0.05515559925684876, -0.2310440960125763, -0.2978861594416706, 0.400634566622496, 0.325454414122532, -0.2351258854978391, -0.09144914352856205, 0.1977161708750055, -0.4610541509870139, -0.05346348988081876, -0.4635699878772121, 0.2043772821103312, -0.4130193684235006, -0.2188825991292715, 0.2764573997165198, -0.06491345303820953, -0.3901015287879339, 0.004707963440896912, -0.04892784615463008, 0.05263916009694297, -0.405813181728969, 0.4152203290328478, 0.06737069718882938, 0.2156770721616582, -0.3401463580504741, -0.1518181262779134, 0.1754080740154759, -0.4120411467328859, 0.2193654583391573, -0.4994609095153683, -0.3297323793311289, -0.2047101313735872, -0.2240073060262983, -0.04096474390932746, -0.2884234104089549, -0.3635493304701355, 0.2720507797851021, 0.1038978095829948, 0.5053057248679331, 0.3032148510977891, -0.3752686361237469, 0.5012237887488495, 0.06006076968104695, -0.5263147299276008, -0.3059296399045745, 0.08700841340272353, 0.152800073532865, -0.1711275374246629, -0.6524331311211893, -0.6294063523359283, 0.3845213214454495, -0.6640812353967686, 0.5924920272587437, -0.1065141762499976, 0.355877233948777, -0.4223316800944034, 0.1715804694193256, -0.06171622138722808, -0.2537887115346543, -0.03112439161032429, -0.172289672422151, -0.1440449533232712, 0.2390462795437584, 0.2680198478147844, 0.3700239718411242, 0.4057985870433324, 0.3942453966377009, 0.1835337830876372, -0.03273752665131715, 0.29572325473821, 0.02290393720279631, 0.004888699435089397, 0.2673670614568584, -0.3248460068820021, 0.009932993073849556, -0.4765511893084158, 0.193483752177162, -0.2917923352056053, -0.2158152191102462, 0.1960045447212593, -0.2005071074671939, -0.3480701715909271, 0.3166079268868118, -0.2358750033825053, 0.06850066295289464, 0.1399863789044257, -0.09643669640479458, -0.1667466287811969, -0.3612270950624845, 0.1309604638400304, 0.3173715378238687, 0.1380985223865595, -0.4713747552462736, 0.09677957305534723, -0.0865831666563559, 0.3938193148438909, 0.03846976838003369, -0.2076881949840571, 0.02936761484933822, -0.4961952078116831, -0.3952220473178389, -0.4108172111448767, 0.00884285158611408, 0.3694331820741996, -0.2256836998167052, -0.3837458954425502, 0.3745075095749829, 0.4919566669622671, -0.1584760203821895, -0.3031162777071806, 0.1669815223382925, 0.1443926065983221, -0.1838177689527926, 0.01928674835082863, 0.4505821733339986, 0.4288052524951801, -0.414317192173175, -0.163563885375491, -0.30227991421922, -0.2696209078508195, 0.3787287213894176, -0.6272867618161119, 0.4527655860532665, 0.269369569055144, 0.2782331857822862, -0.1362919891196078, 0.3023016747690327, -0.3838718816766029, 0.00753460006366962, -0.122183425326942, -0.390341696108184, 0.3273971381117995, -0.1019635121377767, 0.239467695227127, 0.08763009954336919, -0.0583495317366379, 0.2920485689539871, 0.0615020824862221, 0.3046256034605325, -0.3492283594207726, 0.1662445159781237, -0.03225949767631046, 0.3524387319986061, 0.4455015112565127, 0.4877082587812439, 0.2515942932812471, 0.3883366486468989, -0.476164161216544, -0.4448435995470935, -0.3759636221807281, -0.20430991365775, 0.4896789719302575, -0.3228923542531638, -0.2943571437123963, 0.4528267886269962, 0.3269584867297478, 0.02901246004226266, 0.3097939197019646, -0.4847209043729682, -0.3479287847168412, -0.2944056977491852, -0.2759303986536096, 0.3686732101636756, 0.186769671703352, 0.4572933471151471, -0.04372713664118393, -0.06911024212624216, 0.3291429646960283, 0.4239431604448435, -0.3391266651262724, -0.4604026392191921, 0.09056009574379097, 0.06210157904269331, 0.3841879770779499, 0.06573122927489657, -0.05661115485315407, 0.1490079978941398, -0.03692259716530903, -0.02878043500240063, 0.2077712397675546, 0.08087979319739011, 0.2683309467262801, 0.1831028550841631, 0.2584577853782465, 0.4790383002338289, 0.1399441857755936, 0.08338093355410686, 0.001721410336007196, -0.04343842186027478, 0.1026515927231324, 0.1599632118103427, 0.1528804460081741, 0.3225915478257432, 0.02765629572891541, -0.1603657802189623, 0.2789828014303018, 0.486863597139946, 0.2813268340709422, 0.1036372757209295, 0.4162116237629651, 0.4425272831909784, 0.1410899045179981, 0.004031468353432066, 0.001792427094209215, 0.02984930451373214, -0.4328880795129875, 0.4535060545450129, -0.3279941420032236, 0.02190582143784096, -0.07322279343997262, 0.3667174600375432, -0.3967391624565885, -0.3035205345617237, 0.0523193965443966, 0.3607439854465164, -0.3281754184645486, -0.309251878787415, -0.05481438038629216, 0.1761821576748892, 0.1389707660483991, -0.4550936501264077, -0.167389050436853, -0.2072122868649718, 0.3688632416859564, 0.3586360504192468, 0.1334209533058969, 0.1505651803579534, 0.3451470828132002, -0.08593365931486503, -0.2467417684946883, 0.2620482305769652, -0.1417614475267933, 0.3941699339224968, -0.240160467378767, 0.3600057664027321, -0.07945826811732164, -0.1744255132598939, 0.313039040195261, 0.0955036349061138, 0.3489381829222987, -0.2591994486783307, -0.04230213371702166, 0.4475556605621023, -0.06653174979293525, -0.4838497875457697, 0.3136511977314423, 0.1103970656365413, -0.2926446636364258, -0.2394671129712883, -0.2082306468945931, 0.3429639615956397, -0.1876066622719136, 0.1090841572503952, -0.3660994419204743, -0.3248593177947515, -0.01934256788394954, 0.2738112935683278, 0.3252275059471343, -0.1704718552251593, -0.3206781003526738, -0.4205174069087776, -0.4166118444842543, 0.04044930129325572, 0.4793620214156922, -0.1522116605885421, -0.09667502727544784, -0.1004114990564587, 0.1722980307974875, -0.282282241749365, 0.486833420260083, 0.0159508561500683, -0.03951851532956453, -0.05600907551261648, -0.02955902392489795, 0.3949501486005961, -0.03352284316603227, -0.2162473475170542, 0.003559023376348891, 0.1715061243956472, 0.04224994897015855, 0.2883500562926522, 0.01550585800572574, 0.3456535957966249, -0.09424796122789758, 0.1522933052164937, -0.4820795161566136, 0.3817900716707995, -0.07731678410308285, 0.3404743959384385, -0.2366264088580286, 0.07399042290119755, 0.4485393207129868, -0.2090080128243308, -0.3853265165471155, 0.2585262614542096, 0.1274077995183097, -0.05134668620670881, -0.2606489154047852, -0.100255428993129, 0.3446436875375794, -0.1867985203359602, 0.2166944340084855, -0.3407694568225301, 0.1807441833204932, -0.1507836184534569, -0.2788925673830278, -0.3736959680961389, 0.1734221899337506, 0.07636139775286326, 0.3808688134789325, -0.394747333944241, -0.050525657092833, -0.06076501218456009, 0.4665312448185113, 0.3136136185397293, -0.5202450703334656, 0.554084148348662, 0.2242072406848617, -1.364921837544192e-05, 0.3008619184503074, 0.3895396840093567, -0.405367823244807, 0.2193258005559112, -0.3398050958002709, -0.3137672939984258, 0.1184008807634142, 0.3378800934006089, 0.2715158776945228, 0.3852677421474667, -0.3704644948087287, 0.1176728025123271, -0.338327122875982, 0.4281518043863283, 0.2776818609721324, 0.4113291800717574, -0.2682615907508622, 0.3917487430432711, -0.412115583155358, 0.2823718538425732, 0.0320772992130729, 0.4504253314577161, 0.392397792959771, 0.4509477889868188, -0.1715278968548066, 0.3480773274079325, 0.2755703170670059, -0.1411878199508357, 0.3216773456994804, -0.01801708364766885, 0.311835473781375, 0.1357517599294669, -0.1088763669174521, 0.3729389169593057, -0.1381436868765309, 0.09173492785890344, -0.4028699784235117, 0.482129241358919, -0.09478120350889471, 0.3677412690856224, 0.385744365104052, 0.0213685937244655, -0.1026024434608244, -0.4551579749172769, -0.08415759177488098, -0.3211990205249007, 0.447983516544915, 0.1720338870063584, -0.4364328053836875, -0.4562995315016421, -0.08673305748849615, 0.04688818421155133, 0.4775032041278735, -0.1208469013989331, 0.0436160131430237, -0.1925334501782099, -0.2861954358070952, -0.1418001458519035, 0.2806972904458841, -0.5499022634543266, 0.4377612936152993, -0.1835716824315406, 0.1276195973157514, -0.3569000409885634, -0.1178492085205475, -0.43447626729827, 0.2923653539177242, -0.02606088350307709, 0.4813269410653038, -0.308378432912189, -0.257376192393771, 0.3801637531948241, 0.3077870892107425, 0.2411605476618308, 0.409484588736278, -0.3608532183754612, 0.4115554749075608, 0.3925905504089536, 0.3003459584527368, 0.4153998042548785, 0.4327856011881651, -0.3094125471773165, 0.05083344483321228, 0.4263101666822611, 0.1375617127109141, -0.392972825045219, -0.2536110890813223, 0.329110411381866, -0.06120877785664458, 0.4252798361355811, 0.3423364557522984, -0.247461814315739, -0.08399376579746309, -0.008685739947802307, -0.04259121722662418, 0.279671550160121, 0.006712449484836513, -0.2524585415387799, -0.2701429397649928, 0.01115417970918451, -0.05331741264383569, 0.03267195834072616, -0.1059157818170528, -0.3279700490860465, -0.2800000865358575, -0.1679933090530484, -0.157736659005922, -0.4015502721667948, -0.2842148876341706, -0.3216754082419477, -0.442348023569083, -0.3393914180836145, -0.02112001520104761, 0.08729607329238073, -0.4288283762554844, -0.3828335531749115, 0.189042400633997, -0.1636686264499386, 0.4400119002447627, -0.3495061750116777, -0.2483433210835899, 0.2747042539932564, -0.1028077273163152, 0.173086316145751, -0.2221515232987966, 0.3429561807379146, -0.05051680248871524, 0.2758216236562502, -0.3928672999057317, 0.1808469889486128, -0.2137174436946291, 0.05078654616969952, -0.2826588377812559, 0.1800104212318674, 0.2147777481326594, -0.05320232335203155, -0.48944843672079, -0.4466924534520142, 0.05313270170269394, -0.2750227603945427, -0.2639920133872117, 0.1057027238589716, -0.108380354049918, 0.2092216258426406, -0.2932367990896322, -0.02155673545805345, 0.3241772292294434, 0.4173444928216489, 0.3082341997922557, 0.2666979766295748, -0.4343692487731433, -0.4352225316386775, 0.05155518024766592, -0.0335448042180132, 0.23732697252991, 0.3277266439179548, -0.1753115768895073, -0.3104519740727041, 0.1106105570730803, -0.06953689063411994, 0.3707992759862911, 0.3998297065961313, 0.4802857607923326, -0.4159215646559536, 0.07825667550403496, 0.1956242593372549, 0.03279008963061084, 0.08744266515423732, 0.2477966375075491, -0.4133469373827181, 0.3216364700889512, 0.4839262519204975, 0.195878735388392, -0.2858045742525243, 0.1947412558392635, 0.3995276801500448, 0.1927328438656908, 0.01743597830725412, 0.3215635862528346, 0.005055250259035228, -0.2133182130284925, 0.3788267901326311, 0.06841175894504491, 0.3337820272115209, -0.1515395547142427, -0.2005606569350095, 0.1508578412313876, 0.1803267254867877, -0.02451011991691635, -0.2123905276819517, -0.3929097857476503, -0.1378360544043019, -0.3314844457803234, -0.4228992414220882, -0.05226285894390732, 0.2454619666029621, 0.2867841675111378, 0.4723168852978708, -0.1571314567679913, 0.03032160990728614, -0.454602037029009, -0.3456901083139562, 0.01765053826356427, 0.2604471609040798, -0.1245124182905389, -0.2938173899297047, 0.1615115114603728, -0.4338085436533633, 0.2167885928010844, -0.02066192723568819, 0.07023621609911146, -0.4933454124225981, -0.1434495521911651, -0.365320009116698, 0.3428036909749749, 0.2055308724313653, -0.06354926133693628, 0.006679438290502604, -0.1208002798356117, 0.4277695123701215, 0.2811657426791106, -0.0166683227367086, -0.210112438402191, 0.4554817541295112, 0.06791873256113323, 0.2368771018166454, 0.2125326805290371, -0.07667755721089985, 0.1993242792808665, -0.4457382271244442, 0.4286212364115627, 0.4381773122820821, -0.3918263804188478, -0.06758683685251926, 0.08680304797733288, 0.08848885253577145, -0.03226477326641306, -0.3527353888520382, 0.2871587264085371, -0.1573823384243543, 0.2325447203355139, -0.4277128513243943, -0.3018040812686402, -0.3862144460229234, -0.3226985340929358, -0.2662271828382368, 0.2257926810315826, 0.2860360457301119, 0.1548446621287251, -0.481725039217526, 0.1947760518847378, -0.03100050737878004, 0.1392931393426751, -0.6422902253437914, -0.1489025105330943, -0.4093814583978982, 0.2466325702607905, -0.3703790945707442, -0.01779558752491842, -0.00651237446859588, -0.2995985177667599, -0.1234564703322981, -0.2453578782701759, -0.1307072986020559, 0.2780027791105753, 0.3157909193511725, 0.4711654395495154, -0.233045826675902, 0.5062578021198224, 0.1417743073868276, 0.1631426454865459, 0.1493720695668922, 0.3597731715905829, 0.5002942195905533, 0.1959042608253212, -0.2522865784132325, -0.3486089137609159, -0.07095907189462292, -0.4452641731804535, 0.3190737202852376, 0.04137731601548256, 0.2895277448881081, -0.2320779034551596, -0.3629546192767819, 0.3142072240888175, 0.1722522269339544, -0.1993678210765905, 0.07576380464051097, 0.2855422719314425, 0.3115983320454128, -0.4455882501453834, 0.4443470128350614, -0.3107277331946288, -0.1771834208981433, -0.164171088547173, 0.4564117639149661, -0.3860114895789409, -0.1904917829217617, -0.1755886247454847, -0.3766500810610199, 0.5021080999088956, -0.489195058480463, 0.2955439318852746, 0.3254834134214297, 0.4990900461505377, -0.03719837403321202, -0.3838020256689466, -0.2568672676191832, 0.4386014196259068, -0.4033892412607884, 0.5154931427072541, -0.0751406379007787, 0.3876693115793419, -0.3132685028145621, -0.045819765596742, 0.2557926872446639, 0.2369541047649221, 0.548802450176779, -0.1484298295891763, 0.2006966982301739, 0.1587625328719204, -0.2081758912654455, 0.09966499153369571, 0.2521455916498349, 0.2721218374574071, 0.3985236743573616, 0.3351662523980654, 0.360236915903919, -0.4412697548476496, -0.4010471556147061, 0.381934346620823, -0.3337073978275643, -0.341811021264242, 0.1932972464803212, -0.3941146580230249, -0.3343210959237084, -0.3904764516535673, -0.3034176357655628, -0.1728445392441212, -0.4394774567054014, -0.2536606335796698, -0.2105627500966949, 0.00667201332127304, -0.3370845130817427, 0.01964113559557179, -0.4101331359288344, 0.3936988319706632, -0.07790275177820716, 0.4736938946757903, -0.2997540192677425, -0.3899043676862047, 0.1691192563945051, 0.06055191371615598, 0.2481976154950437, 0.0664625823923954, -0.2477016933116983, 0.2272902357121153, -0.07860125492427288, -0.4572961702224899, 0.533563819618126, 0.08813060128714734, 0.4437255200875594, -0.2590804547306598, 0.2838961095599806, 0.08170203555554413, -0.2503963240376878, 0.5282587353316213, 0.2627716890214358, -0.1074711580512993, -0.2257396495907983, 0.2833122432132694, 0.1524320815739529, 0.05409835473248699, -0.1472249411423451, 0.2825333309077213, -0.367050784167318, -0.08697749225213791, 0.1455649410691315, 0.03772010652506388, -0.1043983139798948, 0.3854386899786499, 0.1227239443318415, -0.4458899299392683, -0.07858479647429542, -0.0942061137996227, 0.1241764591147162, 0.1692198469164239, -0.3734482384256268, -0.4433875135083457, 0.2056676218580403, -0.3838248595548543, 0.1570609145352513, 0.1279087420687892, -0.1637776190857923, -0.03639186186299698, -0.2992274928739999, 0.09391251437062388, -0.04312151857466449, 0.4873670090369893, 0.4672383375899919, 0.2682462641772759, 0.3138394117848703, 0.0983039883888811, -0.1203090299946764, -0.3632656735662677, -0.111958194809015, 0.006147185576216851, 0.05335143932763087, -0.4285215465019092, -0.445299350631097, 0.4436624464316584, 0.455161083934438, 0.2133497464067068, -0.4969072905354701, -0.1006043728443814, -0.379803541526107, 0.1245056081677348, -0.4214395945525913, -0.2568539230335101, 0.1669361712698281, 0.2798141137643845, -0.1377162417326557, -0.1704582049946198, -0.09733595675503186, 0.1882392000581337, 0.2939369535336168, 0.1370056475137414, -0.2135721881964115, -0.235962053088191, 0.1281786787025349, -0.241123399230483, -0.4774570566319481, -0.05699475075173756, 0.3503944353790294, -0.08404160794836324, 0.09565035315232809, -0.242815575080791, 0.4025554588604119, -0.3557259577040082, -0.1980749928992108, 0.4592709962459195, -0.4409735183646269, -0.2645898960415179, 0.1993669356343651, -0.4683984031028247, 0.2235178270394383, 0.3029724173338513, 0.1994267782603462, 0.2257640093065016, -0.4967492878743797, -0.1281333336939945, 0.003886308737483649, -0.06973532981101616, 0.1710063529757638, 0.4291035801323895, -0.3978033356099865, -0.07678050597327399, 0.04121791723969927, -0.109344505798696, 0.2424550550698004, -0.3088502640451978, 0.1249697883076875, 0.2784296539891522, 0.1492046082307904, -0.03197127211221471, -0.3096241617756452, -0.2887633106618949, 0.2094675319779047, -0.4284853641542072, -0.1605734967908699, -0.4808725071981886, -0.4659344488596239, -0.08872123648818642, -0.2160514619741829, };



double sigmoid(double x){
    return 1 / (1 + exp(-1 * x));
}

#endif