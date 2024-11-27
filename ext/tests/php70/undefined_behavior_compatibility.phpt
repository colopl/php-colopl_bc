--TEST--
Check undefined behavior compatibility (compatible for amd64)
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
--SKIPIF--
<?php
if ((bool) PHP_ZTS === true) print 'skip ZTS not supported'; 
if (PHP_INT_SIZE !== 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

echo "GNU (compatible) rand\n";
\Colopl\ColoplBc\Php70\srand(PHP_INT_MIN);
for ($i = 0; $i < 512; $i++) {
    echo \Colopl\ColoplBc\Php70\rand(PHP_INT_MIN, PHP_INT_MAX) . PHP_EOL;
}
\Colopl\ColoplBc\Php70\srand(PHP_INT_MAX);
for ($i = 0; $i < 512; $i++) {
    echo \Colopl\ColoplBc\Php70\rand(PHP_INT_MIN, PHP_INT_MAX) . PHP_EOL;
}

echo "mt (broken) rand\n";
\Colopl\ColoplBc\Php70\mt_srand(PHP_INT_MIN);
for ($i = 0; $i < 512; $i++) {
    echo \Colopl\ColoplBc\Php70\mt_rand(PHP_INT_MIN, PHP_INT_MAX) . PHP_EOL;
}
\Colopl\ColoplBc\Php70\mt_srand(PHP_INT_MAX);
for ($i = 0; $i < 512; $i++) {
    echo \Colopl\ColoplBc\Php70\mt_rand(PHP_INT_MIN, PHP_INT_MAX) . PHP_EOL;
}
?>
--EXPECT--
GNU (compatible) rand
0
-1948291122170167296
0
0
0
-5579192487785791488
-3039603657378103296
0
-4099333038905753600
0
-416950803710869504
0
-2494286228334051328
0
0
0
0
0
-6611265939239337984
0
-8922679563985616896
-4742901942244605952
-6691896262230278144
0
-6333152977028644864
-1827253411851010048
-6829160882979733504
-7216203913853140992
0
-5197242764139429888
0
0
0
-3762552550008553472
0
0
-118373000939569152
0
-3827389780035895296
0
0
0
-1840457077621784576
0
-3997137427423559680
-2721663111244283904
0
0
-7936614309012963328
0
0
-7635921836143804416
-5677652414023335936
0
0
-2787433354197204992
-8039620105946005504
-8854011979044487168
-780265231195570176
-8059459255670931456
-4827882706329141248
0
0
0
-4304257207182032896
0
-2302024972266110976
0
0
0
0
-8498777596224339968
-1150383299193995264
0
0
0
-3979084391388282880
0
0
-2692326654956535808
0
-6161689069299433472
-1104876454245564416
0
0
-3129727550310318080
-4999654786551775232
0
-2760367492500029440
0
0
0
0
0
-1114922700578488320
0
-1873512820969046016
0
0
0
-322990379701895168
-5242104255891898368
0
0
-6499525536035897344
0
0
-1255237881979469824
0
-4038740988913516544
0
-3551775329150304256
-977058012768239616
-5052440991181570048
-5763996761504874496
-4127741739307892736
0
-1540279511201873920
-6094673526348840960
0
-7320199528949219328
-6897692523616010240
-84042141931667456
0
0
0
0
-2154794962238046208
0
-2422730037368193024
-3797071124857094144
-4938902877876781056
0
-4714752855435640832
-6412276548182736896
0
-6908768582847430656
0
-6196225665830748160
0
-7848539879985119232
0
-8254378177372094464
0
-5972859157093548032
-4795002893432258560
0
0
0
0
0
0
-7498961599726616576
-6734861775029665792
0
-7780243740995616768
-7933826608949886976
-5448152039152418816
-711666657788952576
0
0
0
-8265269939556843520
-6312344324335992832
0
-5454174450884804608
0
-6908887175484407808
0
-8226185737163243520
0
-7889141185086750720
-9146603903075221504
0
0
-5896091014724059136
-6214127020800999424
-1997962599337033728
0
0
-2599231675447640064
0
0
-874821238319480832
0
-7385337286428196864
0
0
-3610117288725839872
0
0
0
0
0
-8569971635547799552
0
0
0
0
0
0
0
0
0
0
-50571401664397312
-6198696242688557056
0
0
-7803070865108631552
0
-4647765667859136512
0
-4996540815003090944
0
-3378205049200050176
-3158506064576512000
-4954279831459069952
-7855343606398189568
0
-5097639423118409728
0
0
0
-4057508690326978560
0
0
-7133714634527211520
-526037887787991040
0
0
-905411249532043264
-3018788905831890944
0
-1208016477496541184
-9163762125624049664
-2860297628992667648
0
0
-4908831621404164096
0
-314511830791749632
-333225252408524800
-3597932595356106752
0
-5855812149997207552
0
-8469558701781417984
-1586719944601501696
0
0
0
-2820214365666410496
-5817688628708507648
0
0
0
-3165932818945540096
0
-5493204742848905216
0
0
0
-4477661529595969536
0
-7606632461287555072
-4418051609775308800
0
0
-7494169893693030400
-7170698932920516608
-2553012639022186496
0
0
0
-3896362324834910208
0
-3903710833489739776
-3142548981171617792
-5723084234483040256
0
-9157358990810677248
0
-3108662367811010560
-5751675582664409088
-1171423648192397312
0
0
0
0
-6988675135129518080
0
-2142922487220928512
0
0
0
0
-5468570184828583936
0
0
-3739368041666838528
0
0
0
-34238517211037696
0
-6221040045541818368
-4170602807092576256
0
-140216989858660352
-670315004720840704
0
-74203943814561792
-3854386561479081984
-5895187628482887680
0
0
-6658201015432511488
0
-139794631364706304
0
0
-5936023713676787712
-5128830489234243584
-27200053655371776
-6986542288730062848
-6673322658978856960
-2574379560153907200
-3231740436703870976
0
0
0
0
0
-72414170812776448
-3044234989332856832
-1949319844146970624
0
0
-4447397111264182272
-6433671206213255168
-7885152792556339200
-7233922380525993984
0
-2516167317180645376
-3905737972154105856
-3110382622472142848
-7541977518262190080
-1340566942141906944
0
0
-4326474952745156608
0
0
-231933405124624384
-6322213549296844800
0
0
0
-5389334506438131712
0
-1361381229831651328
0
-1948190585575702528
-4716334640351150080
-3209485118766841856
0
0
0
-2983891325889282048
0
-6713238495889981440
-1645672081590845440
-9123601930843586560
0
0
-3805967866142916608
-7108124222945230848
0
0
-8316484718732247040
-938844975771156480
0
0
-5333268793630130176
-481571120709894144
0
-7490862863364390912
-7386827176223440896
-2160188762556989440
-3656825332947746816
0
0
-6793896280901812224
-8273211866663092224
-8237904624149856256
-780009354223943680
0
0
-1059576643900669952
-7026156722016223232
0
0
0
0
-2547184694819028992
-3610296148343914496
0
-431936880909484032
-6090741019932884992
0
0
0
0
-4913175731715964928
0
-7930435981377798144
-7400143421676978176
0
-6093891112156528640
-336960147379191808
-5063798894756888576
0
-3858530131177897984
-2634323130213990400
0
-2873062718472978432
0
0
-8553249790266179584
-4473909128338604032
0
0
0
-3538887025648205824
-5146468854767026176
-5570536926093508608
0
-7199161191564836864
0
0
0
-5527523352610078720
-1822766107329560576
-3400509123605823488
-1217327047471267840
-4962248842629087232
-2107573068128845824
0
-6369222404438228992
0
-8954430616527437824
-2209649262340341760
-2173632499819741184
-3589588710850560000
0
-4419012694607134720
0
0
0
0
0
0
-3468901512812429312
0
-9112603533210812416
0
0
0
0
0
-1831594790563610624
-2542803776637501440
0
0
0
-6403311242039001088
-8616609731932323840
-8050885968372170752
0
-5762460099515777024
0
0
0
-9202610603032576000
-9118056956265758720
-3592709708965740544
-4398251260784934912
0
0
-5881537500861693952
-2926493477434818560
0
0
0
-3445306663295254528
0
-5787393312381992960
-6324665743694626816
0
0
0
0
-5697984565864300544
-1945738734775304192
0
0
0
0
0
-4923087605702066176
0
-7033577575100186624
0
-6078359694370406400
0
-8570077652520534016
0
0
-6111846532175101952
-6346085759212060672
0
-970018295082123264
-3837210016599769088
0
0
-862841913105973248
0
0
0
0
0
-2788627629983399936
0
0
0
0
0
-5941170768124575744
0
0
-8260126578780930048
0
0
0
0
0
0
0
-6201077063319814144
0
0
-8480353328985800704
-4336208353660239872
-7136496175607185408
0
-6236004063788400640
0
0
0
-4867571210621288448
-3362094790081511424
0
0
0
-1905839356178857984
-7014770746534658048
0
-3406874239368691712
-3732569477804457984
-3689954245357862912
-7351724880959635456
-2769324019730612224
-8344511682441117696
-9106781060795990016
0
0
0
0
0
0
-1297495498269130752
-883171298987999232
0
0
0
-2575206642006097920
0
-6795633517863632896
0
0
-2439832683040210944
-4510002452946223104
-711598273319665664
0
-5726810238871404544
0
-7887430224734846976
0
-6024311868867215360
-2396627665684529152
-7400100111226765312
-4152664712972075008
0
-6521239756813107200
-4036073736913289216
0
-7151924496998334464
0
-3774219046984089600
0
0
0
-5618980927376982016
-2943412985701335040
-1482046351737880576
0
0
-4128382222010941440
0
0
-8259666553423790080
-4427370520576524288
0
0
-5300029998776713216
-7679967936945061888
-2365330419387203584
-3964088186656784384
0
0
0
0
0
-2303295483721809920
-6864709579964416000
-7355008701285007360
-8146209082424950784
-4793262040107974656
0
-2697056092554264576
0
0
-7768770629217550336
-5924680559019163648
0
-27444944100655104
0
0
0
0
-5558252881540284416
0
-7531493228214747136
-7511410579613941760
-2940014472569290752
-3608151190136684544
-5968006479704227840
0
0
-8134748237073481728
-4471615143356137472
-4709583175100137472
-8478218060224790528
-7790550107727855616
0
-6119555603334430720
-5922186772158087168
0
-1689445606587629568
-6913755787202396160
-8632771796946386944
-1995145238229811200
0
-7178170389309161472
0
0
0
0
-8055024418469969920
-2138070462666637312
-2917161346724265984
-4389905263155478528
-5330192145347248128
-1225282538084237312
-2677943805914644480
0
0
0
-4352179473018257408
-3182301059569680384
0
0
0
0
0
-5681390856636989440
0
0
0
-5397801673684942848
0
0
0
0
-8981304664985174016
-6089400663719018496
-8286918765162004480
0
0
-7118571146777198592
-9101618415616917504
-8819600640939393024
-2285104373077901312
-5208438515519455232
-821511142168854528
0
0
-5654944870389776384
-4385626427756642304
-8607452561009541120
0
0
0
-7505758119184564224
-308381331912392704
-6375475782331924480
-3963776930376843264
-4016737399009705984
0
-6357511454551179264
-191167027249938432
0
-8160645730227191808
-7584113930574757888
-2145682269996580864
-7918578349767655424
-4450142557439000576
-1209228998303809536
0
0
0
0
0
0
0
-1546949561052823552
0
0
0
0
0
-6815768307367411712
0
-4145923839110414336
-5098154389697200128
-4542797234080579584
-1298027584587563008
0
0
-2539918744025563136
0
-8750701660340224000
-5561981067772035072
0
-7111443554060206080
0
0
-2338214066054496256
-8916892327482490880
-263932964474191872
-3467423880853848064
0
-5800933600509558784
-4192862291147882496
-187480871568146432
-7323000723799539712
0
-6540061523274891264
0
-3718334238198595584
-8054896565883502592
0
-1310730508711231488
0
0
0
-7608737673867427840
-5258969011915325440
-6247444173037764608
0
0
0
0
0
0
0
-2621788070592118784
-2958665977937526784
-6698515459668443136
-2315308352629899264
0
-942567303667515392
0
-9023532534331342848
0
-1300737614462058496
-7123161221276106752
-1651869401571393536
0
0
0
0
-1591775670864707584
-6680934139891286016
-514892061107814400
-5552717098192470016
0
0
-1588314064663085056
0
-5845157959633469440
0
0
0
0
-3240777228283281408
0
-6923114615429660672
0
0
-15050931204784128
-198670103187816448
-5872027919282339840
-1128307674098696192
0
0
0
0
-3435954870224420864
-1046472380852994048
0
-8806158509994409984
-7718796047618670592
0
-6263720613030920192
0
-6819870611020447744
0
-7133945798257016832
0
0
-3755731721035710464
0
0
-6849747520222920704
-3649968967415496704
-9148753903574056960
0
-1349711545990381568
0
-786781254929350656
0
-6323495167537971200
0
-2493070151293861888
0
0
-4922115431264681984
-4220945544698134528
0
0
0
0
0
0
-8501984545225048064
-3474327886163345408
0
0
-1384901647565586432
-1944788121073745920
0
0
0
0
0
-4943399502477262848
0
0
0
-4717534336386072576
-8634482765888225280
0
-1817657467069267968
0
0
-8139983235271622656
-5289442492075212800
-5849687054186708992
-3137556734525046784
-7944919959719641088
0
-7033880129776386048
0
0
0
0
0
0
0
-2694772149205336064
-7678748604319662080
0
-7835405509216698368
-8972179139381428224
-1073882604438028288
0
-4692206596413980672
-8889963913328721920
-3371798435463168
0
-4384126212860018688
0
-196447079655014400
0
-5484402422124118016
-1124018702577041408
0
-1550472877344555008
0
-8255951604281245696
-272020800209420288
-345236549358583808
-6066459697202855936
-1231832655268413440
-7404151193099829248
-9040444689613848576
-1470257027287613440
-8714021694739578880
0
-1894089468118302720
-2185421798500204544
0
-4300644667239694336
-797455270862127104
0
0
-2176497586603556864
-8447423393820573696
0
0
0
0
-1594351903917998080
-1182444027467792384
0
0
0
-2298679828397686784
-8629227194796736512
-56796441363873792
-1331259395824156672
0
0
0
0
-7806184183822286848
0
-2379965632117473280
-7296833833117155328
-3542452836346363904
0
-258883594762584064
-7548862531226304512
-8574699767475470336
0
0
0
-3232836452228268032
0
-543602105005375488
-5412705837267288064
0
-745475420572352512
0
-3473199615434555392
-7149132416068616192
-4862440021782691840
-5779662345479389184
-224440199021592576
-4268295179724652544
0
0
0
0
0
0
0
-6944838379624202240
-7235845374823366656
0
-1263919170525855744
0
-3153649306968260608
0
0
-4209988172592971776
-1670116948085571584
0
0
-2974875579649622016
-6907313267538919424
0
-5265631167616385024
0
-1415844120974327808
0
0
0
0
-5802989257756770304
0
-1908207833074106368
mt (broken) rand
-943257556451590144
0
0
-6433913047231758336
0
0
0
0
-1408320429193428992
0
-2607940460374654976
0
0
-3734827324932096000
-7287714071359520768
-8177202287987392512
-8636372792016633856
0
0
-412005973503246336
0
0
0
0
0
-1977771485632135168
-7898174343123828736
0
0
-2999511391800393728
0
-2631934552871796736
0
0
-6074942352101736448
-8385715528504705024
-5070299937644740608
0
-6904309315872489472
-6905738775477878784
-8930266709613346816
0
0
0
-710580340300709888
-312461482124115968
-5096059931715436544
-3216442605009108992
-7041594011299086336
-4112759209752264704
-2502251193644875776
-1555759286301229056
-6578970671391440896
-646653213843390464
-8123868578106572800
-4853955932364734464
0
-7269721525643640832
-1574209331933478912
0
-4343182384116531200
0
0
-4453706409632071680
0
0
-1219620293719359488
-6729037000512372736
-8876761441558855680
0
-2266798423598432256
-6462358134596829184
-2151132059509194752
0
0
-2093908775705509888
0
0
0
-923259733454880768
-2591621775083175936
-2132928373201043456
0
-7379222842136592384
0
0
-8112408273920983040
-8731772880473292800
0
-2764148549779193856
-3082425211463663616
-6070646646071361536
0
0
0
-4703133036164677632
-3404745997174177792
-1941860070069370880
-2514092487199424512
0
-1193594003086376960
-8514548046120353792
0
0
-8966108684144869376
0
-7340977601474723840
-2771210472446230528
0
0
-6247736771979771904
-9197448713767747584
-2763537229904084992
0
0
0
-621456556273172480
-466748647405518848
-4714515773240901632
0
-6290900763807842304
0
0
0
-2985079820649496576
0
-6674346536232550400
-5055769247238586368
-5597067617685733376
0
-2421593271194091520
0
0
0
-7432169738672799744
-2169259905284833280
0
0
0
0
-8687771679260147712
-7041761274505461760
-578283321675481088
-224805297011556352
-8891538190641397760
-8195617355405983744
0
0
0
-8854664564965376000
0
0
0
0
-7006136771348004864
-2592791913153167360
0
0
-7033232276909457408
-3540968865016053760
0
0
-1581568715445501952
0
-8040059506870190080
-5223709641107570688
0
0
-1181315232752992256
0
-4327800018875449344
0
0
0
0
0
0
-7690217266511609856
0
0
-3346812059491762176
0
0
0
0
0
-3893693054200053760
0
-3884771943269793792
-4339962447134720000
0
0
0
0
-8852453197973815296
0
0
0
0
0
0
0
-4242604145648861184
0
-4421894858180919296
-3901921120617496576
-8591152988322529280
-1230618012747300864
-4634686530370142208
-4681527392160710656
-1303538113527676928
0
0
0
-1375582307267641344
0
-5108242288622960640
0
0
0
0
0
0
0
0
0
0
-4986539519797886976
-5853862441003253760
-1626631280386899968
-1909132152985878528
-3279049724807610368
0
0
-1560032778760749056
-3445513629179314176
0
0
-3468287435568316416
-1715221388958105600
0
-2992662207453462528
-89753572262019072
0
-8478572412206579712
-4970788362894966784
-2716972070654181376
0
0
-2234394510257815552
0
-463612402156437504
-8869319981221806080
0
-3660298587100676096
0
-2911919244650217472
-3910875697932926976
0
-2149742491560050688
-2260340124225110016
0
0
0
-6724316565166096384
-7325877965340803072
0
0
0
-2692363832193449984
0
0
0
-8603098537913221120
-2760775016176943104
0
0
-3302018033085579264
0
-5141795956118781952
-7272195186287968256
0
-2443052259244769280
-7429243062057959424
-1183083419249147904
-8827658016766558208
-7294991120348479488
0
-5695557849212518400
0
-3844420167178125312
0
-7374512310984900608
-7386646615798317056
-7641240208247095296
-2724369000770306048
-4012867358598103040
0
-9141316557815676928
0
-6466537412653744128
0
0
-8530523683783966720
-6225906518366093312
0
0
0
0
-5046272559151054848
-6376820141455310848
0
-5747826261175042048
-1634441953292582912
-1213864204319064064
-6641333622299164672
-1712165957813665792
0
-7947466274030747648
-8854672098338013184
0
0
0
0
0
-8290186067043155968
-6675230199983898624
-2481159030161014784
0
0
0
-4966927118576386048
-9007285377425211392
-4807968454806077440
0
-8131813382840909824
0
0
0
0
0
0
0
0
-5534087316768817152
-5912361003676336128
0
-6069226119997947904
-5352476316874571776
-674210488518705152
-5092690727440220160
-6812844998006931456
0
-8131743830140518400
0
-1936091190256467968
-3836105479860256768
0
0
0
0
-101801797340889088
0
0
-1345826129595858944
0
0
0
0
0
0
0
0
-6641007771720351744
0
0
0
0
-3469002539033165824
-903745798653607936
-1877489926325403648
0
-5352438014356226048
0
-5788717321950330880
-5188158435861987328
-8118431707855585280
0
0
0
-176004040348401664
-8721637934475771904
-5028312577877213184
-3409286799808266240
-4531323100100296704
0
0
-8182637457431330816
-1209799662018494464
0
0
0
-3687185864057683968
-8691303310608564224
-2255080575993905152
-2267211951639625728
0
-831237645366460416
-8768129847353409536
-6606747384535842816
-7982835312533438464
0
0
0
-854142551337205760
0
0
-1877819693914390528
0
0
-9002360836643094528
-562677575185334272
0
-811386985129181184
-2942938726822576128
-7593932105554329600
0
0
0
0
0
0
0
-7413048277983035392
0
-2610459046376898560
-1725976287715000320
-3234028340012646400
-6185556004994809856
-316342680860753920
0
0
-6777386933422129152
-3470566362395443200
0
0
-5558806571544215552
-7191518709578465280
-5798993306083917824
-3414996718310129664
0
-809262866103271424
-3357202985540124672
0
-5247957059365765120
0
0
0
-4211227090269241344
-6931948151976755200
0
0
-5288460628191608832
-8391167912177565696
-414569038186807296
0
0
-3981831237722439680
-5396224596053590016
-6388212559387820032
0
0
0
0
-671914854268796928
-5259522083443965952
-4102026309817860096
-5783825002012868608
0
-5752646597460623360
-6651166941333422080
-4478984542731894784
-7055291941906284544
0
0
0
0
0
0
-3832697191382646784
0
-679758899070369792
0
-6218985109849112576
0
0
-6551402435959586816
-6180417952798474240
0
0
-2663474928677814272
0
0
0
-4793041673925951488
-4490958267308048384
0
-7515274400912900096
0
0
0
0
-7422378991354380288
0
0
-1253330375334166528
-5230406122187259904
-564334599337934848
0
0
-8892443397948702720
-3992582769224974336
-8744195446311747584
0
0
-3703134795614650368
0
0
0
0
0
0
0
0
-1581873915821555712
-5158777406403641344
-3242929522294652928
-529169073565597696
-5870355974413352960
-3992951054080671744
-8040056148205764608
-2153299583474597888
-6017329059497771008
-186549087003213824
0
-7400723723298275328
-186574530389475328
-3327761500122120192
-5418966035569049600
-2286965820493922304
-9066235031923458048
0
0
-2630808670144823296
-2266880629272477696
-2334546335782404096
0
-9211501554702483456
-1582140049175085056
-9147389151945949184
-8646305195946409984
-5623259805725491200
0
0
-257414672997679104
0
-7030545895124893696
-5474833793334378496
0
-6146358320975314944
0
-2600499687232372736
0
-8628762934601842688
0
0
-5253321129921085440
0
-1585711056783671296
0
-9122599975102971904
-7568230669067747328
-8497261532898394112
-3768173536917585920
0
0
0
0
-2797229032138080256
-5269008566689202176
-5799545535798968320
0
0
-2175179332291395584
-221834571212128256
-6121258240039714816
0
-8066122210235908096
-3359749179592146944
-1211188688801759232
-5133846315251269632
0
-2325403535820193792
0
-4597149572765057024
-3091381798823788544
0
-301195215971024896
0
0
-3372876214556426240
-318687913392996352
-981684199352172544
-4065967173149917184
-8529564342708862976
0
-5568425228113018880
0
0
0
0
-6280346637592166400
-5503447561245032448
0
0
0
-1181128058078232576
-5239795290063503360
-219709653322301440
-4797760743472627712
0
-6035794592782090240
-1207155680151076864
-6664477955516792832
0
0
0
-3993913058035499008
0
0
0
-6174568611068248064
0
0
-4712582685770383360
-5384606494769545216
-675705893051957248
-7918114811127267328
-820025066304569344
-5239542033021927424
-2509029528211292160
-5786701994446028800
-395687502960132096
-1561560988254208000
0
-3904083336003321856
0
0
-6490825006036025344
-8756941689475563520
0
-8134637366787702784
0
-5511113407853494272
0
0
-7436024557720305664
0
0
-111889799345864704
-8012865036672303104
-2819970789481119744
-2506794298381434880
0
-7301322383179120640
0
-6393554089954770944
-8282608567911972864
-3795233119372574720
-3132719476068319232
-4063716910934523904
-5500059098336460800
-1634953415178059776
0
-6095543995960655872
-5171434434506784768
0
-2504859613183082496
0
0
-7118239815820115968
0
0
-1226572814979432448
0
-6541565028116463616
0
-4868791616988512256
-552845072194863104
-655400473087442944
-5769718551296344064
-2160282324124565504
-2946649209199132672
-7302312536349605888
0
0
0
0
-4143447137629241344
-6903764516450861056
0
0
0
-2472811890070257664
0
-262731181085163520
0
0
-9149504758346678272
0
0
-3808271798269640704
0
0
-4292152632132239360
-6338119866188300288
-1236784683841224704
0
0
-793928252308586496
-2617628532204896256
0
-6053391666499289088
-4423902325895069696
0
-8761042541429653504
0
0
-4970290232587976704
0
0
-5473838563512549376
0
-494537841724882944
-4407744762006732800
-911363129311494144
-1609464270334984192
0
0
0
0
-493832668224421888
-5844681905458380800
0
-4784212870432817152
-88536309810855936
-595365059446702080
-716222827816026112
0
-2892447005391454208
-6230205840758931456
-5913794938637647872
0
-6432024249463996416
-9048255457008812032
-6743904364814925824
-4783035542587572224
-4516263183823929344
-7848238330331267072
0
0
0
0
-5991032014126448640
-4859015498328702976
0
0
0
-837640445172383744
-607658656007192576
0
-9138656246482599936
-8323435839832981504
-8948164894897733632
-3126344851247857664
0
-7290495114223288320
-1980941377655013376
-778067779898048512
-8250967071395938304
-4494688403454951424
0
-2361793677027704832
0
0
-698396866701688832
0
-5806496424971468800
-6861518075666104320
0
0
0
0
-1419645879995858944
0
0
-4681125821308469248
-4637956022684614656
-7980267592105328640
-3658050137361481728
0
-3810891470622162944
-3201908117851865088
-4080948199466991616
0
-7267589821705486336
0
-5935402979233366016
0
-5069084848447029248
-8438475173056741376
0
-4433205225058205696
-6911431170513174528
0
-1896898136211718144
-4892663955752747008
-3045369066857431040
0
0
0
-4206234233737314304
-486618798204387328
-1127825297731747840
-8136176511267897344
-7691832028186017792
-5048111792996220928
0
-7310317745503993856
-1496067951080505344
0
-6059950957733609472
-8886027790550302720
-5107411942595624960
-5973780719636316160
-1097736766900666368
0
0
-3482219776900595712
-787007488036700160
-2945832254879825920
0
-5869009046899523584
-6751002167638753280
-5807842845679157248
-6378685780759412736
0
-1179249104375513088
0
0
0
-628100612982046720
-2836846420361740288
0
0
-7730672181947924480
0
0
-470668964704288768
-4628982830980923392
-4243723895162535936
0
-3925287031197925376
-7904609913530220544
-7521053571467444224
-3403713246518050816
-9099717600530661376
-5931065637790023680
0
0
-7375155834524794880
0
0
-7259312285854728192
-9212862500989566976
-6657413653437874176
-4549355202464972800
-1303912050560335872
-8037770564179329024
0
0
-4554396222760157184
-6014519867418345472
-5367046022763642880
0
0
-9062232259842539520
-2278658958606467072
0
0
0
-5370560044746145792
0
-7696443208154218496
-631424874719477760
-7028155608385716224
-1674507358144823296
-4969767363269361664
0
-5150721834462740480
0
-4306362909388177408
-8246772108118458368
-8943098877892886528
0
-5494658048112721920
0
-5623702505184559104
0
-8890240741150818304
-1158546185187753984
0
0
-1825499252718043136
-5529952122256228352
0
-7162120010624663552
0
0
-6782794151218905088
0
-710495437387202560
-4897516684681674752
-4301346327456907264
-1311153382601261056
-6438608185220661248
-5710352542668423168
-2861175408638820352
0
-4069531403300110336
0
0
0
-8392207156824244224
0
0
0
-5987413864956821504
0
0
0
-8432585596072886272
0
0
0
0
0
0
-677328634775601152
0
0
-2632695689796124672
0
-9187579961253298176
-8904391163742519296
-4423616865188708352
0
-5691866702778728448
-3027092452004593664
-507055128772083712
-2415393623341268992
-4888068787422625792
0
-7985556930229698560
0
0
0
0
-2552807202146484224
-4371711103084789760
0
0
-6254010353399627776
-6704329307788935168
0
-2253110715013398528
0
-8843019293718216704
0
-2857680061174120448
0
-7537126601809461248
-2577024323705372672
0
0
0
0
-3942649513801416704
-168037623338631168
0
-2893238516324499456
-4288967106428403712
0
0
0
-6638794136986058752
0
-4496510414481260544
0
-6167637092198252544
0
-3099148628703313920
0
0
-6860806648693260288
0
-4659362320076505088
0
-7314666288222044160
-1412033488550363136
-2046752586006003712
0
-9048223648481017856
-4955874063189803008
