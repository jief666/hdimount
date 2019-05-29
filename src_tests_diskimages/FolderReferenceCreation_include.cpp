#include <inttypes.h>

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

void _fillFile(VOLUME_TYPE volume, const string& currentfile, const DiskImageKind& dik, uint64_t sizewanted)
{
    FD_TYPE fd = COT_Open_T(volume, dik, currentfile);
    uint64_t ofs = 0;
    for (uint64_t i=0 ; i<sizewanted ; ) {
        string valueBlock = stringPrintf("Block %08" PRIu64 " (%08" PRIx64 ")\n", i/512, i/512);
        COT_Write_T(volume, dik, currentfile, fd, valueBlock, MIN(valueBlock.size(), sizewanted-i), &ofs);
        size_t j;
        for (j=MIN(valueBlock.size(), sizewanted-i) ; j<MIN(512, sizewanted-i) ; ) {
            string valueOffset = stringPrintf("Offset %03zu (0x%03zx)\n", j, j);
            if ( j+valueOffset.length() > MIN(512, sizewanted-i) ) {
                string valuePadding;
                while ( valuePadding.length() < MIN(512, sizewanted-i) - j -1 ) valuePadding += ".";
                valuePadding += "\n";
                valuePadding = valuePadding.substr(0, MIN(512, sizewanted-i)-j);
                COT_Write_T(volume, dik, currentfile, fd, valuePadding.data(), valuePadding.size(), &ofs);
                j += valuePadding.length();
                if ( j != MIN(512, sizewanted-i) ) throw stringPrintf("j is supposed to be 512");
            }else{
                COT_Write_T(volume, dik, currentfile, fd, valueOffset.data(), valueOffset.size(), &ofs);
                j += valueOffset.length();
            }
        }
        if ( j != MIN(512, sizewanted-i) ) {
            throw stringPrintf("%s\n", stringPrintf("j(%zu) is supposed to be min(512, %" PRIu64 "-i)=%" PRIu64, j, sizewanted, MIN(512, sizewanted-i)).c_str());
        }
        i += j;
    }
    COT_Close_T(volume, dik, currentfile, fd);
}

void _createFolderReference(VOLUME_TYPE volume, DiskImageTestSize size, const DiskImageKind& dik)
{
    struct timespec ti = { 1000000000, 1000}; // from unix epoch, 9 sep 2001 01:46:40. Add = 2082844800 to get seconds from Apple epoch (1904)
#ifndef DEBUGdsgdssdgf
    {
        string currentfile = COT_File_T(volume, dik, "/file.txt");
        COT_Times_T(volume, dik, currentfile, &ti);
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+creator.txt");
        COT_FileCreator_T(volume, dik, currentfile, "JIEF");
        COT_FileType_T(volume, dik, currentfile, "TXT ");
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+user0_group0.txt");
        COT_OwnerAndGroup_T(volume, dik, currentfile, 0, 0);
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+user501_group1001.txt");
        COT_OwnerAndGroup_T(volume, dik, currentfile, 501, 1001);
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+user502_group1002.txt");
        COT_OwnerAndGroup_T(volume, dik, currentfile, 502, 1002);
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+lotxattrs.txt");
        for (int i=0 ; i<200 ; i++) {
            COT_XAttr_T(volume, dik, currentfile, stringPrintf("xattr_%d", i), stringPrintf("content of xattr_%d", i));
        }
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+RedOrange.txt");
        COT_XAttr_T(volume, dik, currentfile, "com.apple.metadata:_kMDItemUserTags", "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"><plist version=\"1.0\"><array><string>Orange</string><string>Red</string></array></plist>");
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+zerolength.txt", false);
        COT_FileContent_T(volume, dik, currentfile, "");
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+quarantine.txt");
        string value = "0006;56dfe5f5;iPhoto;";
        COT_XAttr_T(volume, dik, currentfile, "com.apple.quarantine", "0006;56dfe5f5;iPhoto;");
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+creationtime0.txt", false);
        COT_FileContent_T(volume, dik, currentfile, "file+creationtime0 content (fsCatalogInfo.createDate = 0)");
        struct timespec ts0 = {0, 0};
        COT_FileCreationTime_T(volume, dik, currentfile, &ts0);
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+modificationtime0.txt", false);
        COT_FileContent_T(volume, dik, currentfile, "file+modificationtime0 content (fsCatalogInfo.contentModDate = 0)");
        struct timespec ts0 = {0, 0};
        COT_ModificationTime_T(volume, dik, currentfile, &ts0); // seems to change the creation time too
        struct timespec ts1 = {1000000000-1, 20046};
        COT_FileCreationTime_T(volume, dik, currentfile, &ts1);
    }
    {
        string currentfile = COT_File_T(volume, dik, "/file+rsrcfork.txt");
    #ifdef CREATION
        COT_FileContent_T(volume, dik, currentfile+"/..namedfork/rsrc", stringPrintf("Content of '%s'", (currentfile+"/..namedfork/rsrc").c_str()));
    #else
        COT_XAttr_T(volume, dik, currentfile, "com.apple.ResourceFork", stringPrintf("Content of '%s'", (currentfile+"/..namedfork/rsrc").c_str()));
    #endif
    }
    {
        string currentfile = COT_File_T(volume, dik, "/HardLink1.txt", false);
        COT_FileContent_T(volume, dik, currentfile, "Content of HardLink1.txt and HardLink2.txt");
        COT_HardLink_T(volume, dik, "/HardLink1.txt", "/HardLink2.txt");
    }

    {
        string currentfile = "/file+4kfile.txt";
        _fillFile(volume, currentfile, dik, 4068);
        COT_Times_T(volume, dik, currentfile, &ti);
        COT_Alias_T(volume, dik, currentfile, stringPrintf("%s alias", currentfile.substr(1).c_str()));
        COT_HardLink_T(volume, dik, currentfile, "/file+4kfileHardlink.txt");
        COT_SymLink_T(volume, dik, currentfile.substr(1), "/file+4kfileSymlink.txt");
    }

    // UTF8 names
    // Note : long file names doesn't appear in fuse yet. It's a bug. (May 2019)

    map<string, string> lower_upper_map;
    if ( dik.fs() == DISKIMAGE_FS_HFS ) {
        lower_upper_map ["abcdefghijklmnopqrstuvwxyzàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþāăąćĉċčďđēĕėęěĝğġģĥħĩīĭįĳĵķĺļľŀłńņňŋōŏőœŕŗřśŝşšţťŧũūŭůűųŵŷÿźżžɓƃƅɔƈɖɗƌǝəɛƒɠɣɩɨƙɯɲɵơƣƥƨʃƭʈưʊʋƴƶʒƹƽǆǆǉǉǌǌǎǐǒǔǖǘ"] = "ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞĀĂĄĆĈĊČĎĐĒĔĖĘĚĜĞĠĢĤĦĨĪĬĮĲĴĶĹĻĽĿŁŃŅŇŊŌŎŐŒŔŖŘŚŜŞŠŢŤŦŨŪŬŮŰŲŴŶŸŹŻŽƁƂƄƆƇƉƊƋƎƏƐƑƓƔƖƗƘƜƝƟƠƢƤƧƩƬƮƯƱƲƳƵƷƸƼǄǅǇǈǊǋǍǏǑǓǕǗ";
        lower_upper_map ["ǚǜǟǡǣǥǧǩǫǭǯǳǳǵǹǻǽǿȁȃȅȇȉȋȍȏȑȓȕȗșțȟȧȩȫȭȯȱȳ̀́̓ʹ;ά·έήίόύώαβγδεζηθικλμνξοπρστυφχψωϊϋϣϥϧϩϫϭϯѐёђѓєѕіїјљњћќѝўџабвгдежзийклмнопрстуфхцчшщъыьэюяѡѣѥѧѩѫѭѯѱѳѵѷѹѻѽѿҁґғҕҗҙқҝҟҡңҥҧҩҫҭүұҳҵҷҹһҽҿӂӄӈӌӑӓӗӝӟ"] = "ǙǛǞǠǢǤǦǨǪǬǮǱǲǴǸǺǼǾȀȂȄȆȈȊȌȎȐȒȔȖȘȚȞȦȨȪȬȮȰȲ̀́̓ʹ;Ά·ΈΉΊΌΎΏΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩΪΫϢϤϦϨϪϬϮЀЁЂЃЄЅІЇЈЉЊЋЌЍЎЏАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯѠѢѤѦѨѪѬѮѰѲѴѶѸѺѼѾҀҐҒҔҖҘҚҜҞҠҢҤҦҨҪҬҮҰҲҴҶҸҺҼҾӁӃӇӋӐӒӖӜӞ";
        lower_upper_map ["ӣӥӧӭӯӱӳӵӹաբգդեզէըթժիլխծկհձղճմյնշոչպջռսվտրցւփքօֆႠႡႢႣႤႥႦႧႨႩႪႫႬႭႮႯႰႱႲႳႴႵႶႷႸႹႺႻႼႽႾႿჀჁჂჃჄჅḁḃḅḇḉḋḍḏḑḓḕḗḙḛḝḟḡḣḥḧḩḫḭḯḱḳḵḷḹḻḽḿṁṃṅṇṉṋṍṏṑṓṕṗṙṛṝṟṡṣṥṧṩṫṭṯṱṳṵṷṹṻṽṿẁẃẅẇẉẋẍẏ"] = "ӢӤӦӬӮӰӲӴӸԱԲԳԴԵԶԷԸԹԺԻԼԽԾԿՀՁՂՃՄՅՆՇՈՉՊՋՌՍՎՏՐՑՒՓՔՕՖაბგდევზთიკლმნოპჟრსტუფქღყშჩცძწჭხჯჰჱჲჳჴჵḀḂḄḆḈḊḌḎḐḒḔḖḘḚḜḞḠḢḤḦḨḪḬḮḰḲḴḶḸḺḼḾṀṂṄṆṈṊṌṎṐṒṔṖṘṚṜṞṠṢṤṦṨṪṬṮṰṲṴṶṸṺṼṾẀẂẄẆẈẊẌẎ";
        lower_upper_map ["ẑẓẕạảấầẩẫậắằẳẵặẹẻẽếềểễệỉịọỏốồổỗộớờởỡợụủứừửữựỳỵỷỹἀἁἂἃἄἅἆἇἐἑἒἓἔἕἠἡἢἣἤἥἦἧἰἱἲἳἴἵἶἷὀὁὂὃὄὅὑὓὕὗὠὡὢὣὤὥὦ"] = "ẐẒẔẠẢẤẦẨẪẬẮẰẲẴẶẸẺẼẾỀỂỄỆỈỊỌỎỐỒỔỖỘỚỜỞỠỢỤỦỨỪỬỮỰỲỴỶỸἈἉἊἋἌἍἎἏἘἙἚἛἜἝἨἩἪἫἬἭἮἯἸἹἺἻἼἽἾἿὈὉὊὋὌὍὙὛὝὟὨὩὪὫὬὭὮ";
    }
    if ( dik.fs() == DISKIMAGE_FS_APFS ) {
        lower_upper_map ["abcdefghijklmnopqrstuvwxyzàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþāăąćĉċčďđēĕėęěĝğġģĥħĩīĭįĳĵķĺļľŀłńņňŋōŏőœŕŗřśŝşšţťŧũūŭůűųŵŷÿźżžsɓƃƅɔƈɖɗƌǝəɛƒɠɣɩɨƙɯɲɵơƣƥʀƨʃƭʈưʊʋƴƶʒƹƽǆǆǉǉǌǌǎǐǒǔǖǘǚǜǟǡǣǥǧǩǫǭǯǳǳǵƕƿǹǻǽǿȁȃȅȇȉȋȍȏȑȓȕȗșțȝȟƞȣȥȧȩȫȭȯȱȳⱥȼƚⱦɂƀʉʌɇɉɋɍɏ̀́̓ͱͳʹͷ;ϳά·έήίόύώαβγδεζηθͅκλµ"] = "ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞĀĂĄĆĈĊČĎĐĒĔĖĘĚĜĞĠĢĤĦĨĪĬĮĲĴĶĹĻĽĿŁŃŅŇŊŌŎŐŒŔŖŘŚŜŞŠŢŤŦŨŪŬŮŰŲŴŶŸŹŻŽſƁƂƄƆƇƉƊƋƎƏƐƑƓƔƖƗƘƜƝƟƠƢƤƦƧƩƬƮƯƱƲƳƵƷƸƼǄǅǇǈǊǋǍǏǑǓǕǗǙǛǞǠǢǤǦǨǪǬǮǱǲǴǶǷǸǺǼǾȀȂȄȆȈȊȌȎȐȒȔȖȘȚȜȞȠȢȤȦȨȪȬȮȰȲȺȻȽȾɁɃɄɅɆɈɊɌɎ̀́̓ͰͲʹͶ;ͿΆ·ΈΉΊΌΎΏΑΒΓΔΕΖΗΘΙΚΛΜ";
        lower_upper_map ["νξοπρστυφχψωϊϋͅµσϗβθφπϙϛϝϟϡϣϥϧϩϫϭϯκρθεϸϲϻͻͼͽѐёђѓєѕіїјљњћќѝўџабвгдежзийклмнопрстуфхцчшщъыьэюяѡѣѥѧѩѫѭѯѱѳѵѷѹѻѽѿҁҋҍҏґғҕҗҙқҝҟҡңҥҧҩҫҭүұҳҵҷҹһҽҿӏӂӄӆӈӊӌӎӑӓӕӗәӛӝӟӡӣӥӧөӫӭӯӱӳӵӷӹӻӽӿԁԃԅԇԉԋԍԏԑԓԕԗԙԛԝԟԡԣԥԧԩԫԭԯաբգդեզէըթժիլխծկհձղճմյնշոչպջռսվտրցւփքօֆⴀⴁⴂⴃⴄⴅⴆⴇⴈⴉⴊⴋⴌⴍⴎⴏⴐⴑⴒⴓⴔⴕⴖⴗⴘ"] = "ΝΞΟΠΡΣΤΥΦΧΨΩΪΫιμςϏϐϑϕϖϘϚϜϞϠϢϤϦϨϪϬϮϰϱϴϵϷϹϺϽϾϿЀЁЂЃЄЅІЇЈЉЊЋЌЍЎЏАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯѠѢѤѦѨѪѬѮѰѲѴѶѸѺѼѾҀҊҌҎҐҒҔҖҘҚҜҞҠҢҤҦҨҪҬҮҰҲҴҶҸҺҼҾӀӁӃӅӇӉӋӍӐӒӔӖӘӚӜӞӠӢӤӦӨӪӬӮӰӲӴӶӸӺӼӾԀԂԄԆԈԊԌԎԐԒԔԖԘԚԜԞԠԢԤԦԨԪԬԮԱԲԳԴԵԶԷԸԹԺԻԼԽԾԿՀՁՂՃՄՅՆՇՈՉՊՋՌՍՎՏՐՑՒՓՔՕՖႠႡႢႣႤႥႦႧႨႩႪႫႬႭႮႯႰႱႲႳႴႵႶႷႸ";
        lower_upper_map ["ⴙⴚⴛⴜⴝⴞⴟⴠⴡⴢⴣⴤⴥⴧⴭꭰꭱꭲꭳꭴꭵꭶꭷꭸꭹꭺꭻꭼꭽꭾꭿꮀꮁꮂꮃꮄꮅꮆꮇꮈꮉꮊꮋꮌꮍꮎꮏꮐꮑꮒꮓꮔꮕꮖꮗꮘꮙꮚꮛꮜꮝꮞꮟꮠꮡꮢꮣꮤꮥꮦꮧꮨꮩꮪꮫꮬꮭꮮꮯꮰꮱꮲꮳꮴꮵꮶꮷꮸꮹꮺꮻꮼꮽꮾꮿᏸᏹᏺᏻᏼᏽвдосттъѣḁḃḅḇḉḋḍḏḑḓḕḗḙḛḝḟḡḣḥḧḩḫḭḯḱḳḵḷḹḻḽḿṁṃṅṇṉṋṍṏṑṓṕṗṙṛṝṟṡṣṥṧṩṫṭṯṱṳṵṷṹṻṽṿẁẃẅẇẉẋẍẏẑẓẕṡßạảấầẩẫậắằẳẵặẹẻẽếềểễệỉịọỏốồổỗộớờởỡợụủứừửữựỳỵỷỹỻỽỿἀἁἂἃἄἅἆἇἐἑἒἓἔἕἠἡἢἣἤἥἦ"] = "ႹႺႻႼႽႾႿჀჁჂჃჄჅჇჍᎠᎡᎢᎣᎤᎥᎦᎧᎨᎩᎪᎫᎬᎭᎮᎯᎰᎱᎲᎳᎴᎵᎶᎷᎸᎹᎺᎻᎼᎽᎾᎿᏀᏁᏂᏃᏄᏅᏆᏇᏈᏉᏊᏋᏌᏍᏎᏏᏐᏑᏒᏓᏔᏕᏖᏗᏘᏙᏚᏛᏜᏝᏞᏟᏠᏡᏢᏣᏤᏥᏦᏧᏨᏩᏪᏫᏬᏭᏮᏯᏰᏱᏲᏳᏴᏵᲀᲁᲂᲃᲄᲅᲆᲇḀḂḄḆḈḊḌḎḐḒḔḖḘḚḜḞḠḢḤḦḨḪḬḮḰḲḴḶḸḺḼḾṀṂṄṆṈṊṌṎṐṒṔṖṘṚṜṞṠṢṤṦṨṪṬṮṰṲṴṶṸṺṼṾẀẂẄẆẈẊẌẎẐẒẔẛẞẠẢẤẦẨẪẬẮẰẲẴẶẸẺẼẾỀỂỄỆỈỊỌỎỐỒỔỖỘỚỜỞỠỢỤỦỨỪỬỮỰỲỴỶỸỺỼỾἈἉἊἋἌἍἎἏἘἙἚἛἜἝἨἩἪἫἬἭἮ";
        lower_upper_map ["ἧἰἱἲἳἴἵἶἷὀὁὂὃὄὅὑὓὕὗὠὡὢὣὤὥὦὧάέήίόύώᾀᾁᾂᾃᾄᾅᾆᾇᾐᾑᾒᾓᾔᾕᾖᾗᾠᾡᾢᾣᾤᾥᾦᾧᾰᾱὰάᾳͅὲέὴήῃΐῐῑὶίΰῠῡὺύῥ΅`ὸόὼώῳ´  ωkåⅎⅰⅱⅲⅳⅴⅵⅶⅷⅸⅹⅺⅻⅼⅽⅾⅿↄⓐⓑⓒⓓⓔⓕⓖⓗⓘⓙⓚⓛⓜⓝⓞⓟⓠⓡⓢⓣⓤⓥⓦⓧⓨⓩⰰⰱⰲⰳⰴⰵⰶⰷⰸⰹⰺⰻⰼⰽⰾⰿⱀⱁⱂⱃⱄⱅⱆⱇⱈⱉⱊⱋⱌⱍⱎⱏⱐⱑⱒⱓⱔⱕⱖⱗⱘⱙⱚⱛⱜⱝⱞⱡɫᵽɽⱨⱪⱬɑɱɐɒⱳⱶȿɀⲁⲃⲅⲇⲉⲋⲍⲏⲑⲓⲕⲗⲙⲛⲝⲟⲡⲣⲥⲧⲩⲫⲭⲯⲱⲳⲵⲷⲹⲻⲽⲿⳁⳃⳅⳇⳉⳋⳍⳏⳑⳓⳕⳗⳙⳛⳝⳟⳡⳣⳬⳮⳳ〈〉ꙁ"] = "ἯἸἹἺἻἼἽἾἿὈὉὊὋὌὍὙὛὝὟὨὩὪὫὬὭὮὯάέήίόύώᾈᾉᾊᾋᾌᾍᾎᾏᾘᾙᾚᾛᾜᾝᾞᾟᾨᾩᾪᾫᾬᾭᾮᾯᾸᾹᾺΆᾼιῈΈῊΉῌΐῘῙῚΊΰῨῩῪΎῬ΅`ῸΌῺΏῼ´  ΩKÅℲⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩⅪⅫⅬⅭⅮⅯↃⒶⒷⒸⒹⒺⒻⒼⒽⒾⒿⓀⓁⓂⓃⓄⓅⓆⓇⓈⓉⓊⓋⓌⓍⓎⓏⰀⰁⰂⰃⰄⰅⰆⰇⰈⰉⰊⰋⰌⰍⰎⰏⰐⰑⰒⰓⰔⰕⰖⰗⰘⰙⰚⰛⰜⰝⰞⰟⰠⰡⰢⰣⰤⰥⰦⰧⰨⰩⰪⰫⰬⰭⰮⱠⱢⱣⱤⱧⱩⱫⱭⱮⱯⱰⱲⱵⱾⱿⲀⲂⲄⲆⲈⲊⲌⲎⲐⲒⲔⲖⲘⲚⲜⲞⲠⲢⲤⲦⲨⲪⲬⲮⲰⲲⲴⲶⲸⲺⲼⲾⳀⳂⳄⳆⳈⳊⳌⳎⳐⳒⳔⳖⳘⳚⳜⳞⳠⳢⳫⳭⳲ〈〉Ꙁ";
        lower_upper_map ["ꙃꙅꙇꙉᲈᲈꙍꙏꙑꙓꙕꙗꙙꙛꙝꙟꙡꙣꙥꙧꙩꙫꙭꚁꚃꚅꚇꚉꚋꚍꚏꚑꚓꚕꚗꚙꚛꜣꜥꜧꜩꜫꜭꜯꜳꜵꜷꜹꜻꜽꜿꝁꝃꝅꝇꝉꝋꝍꝏꝑꝓꝕꝗꝙꝛꝝꝟꝡꝣꝥꝧꝩꝫꝭꝯꝺꝼᵹꝿꞁꞃꞅꞇꞌɥꞑꞓꞗꞙꞛꞝꞟꞡꞣꞥꞧꞩɦɜɡɬɪʞʇʝꭓꞵꞷ豈更車賈滑串句龜龜契金喇奈懶癩羅蘿螺裸邏樂洛烙珞落酪駱亂卵欄爛蘭鸞嵐濫藍襤拉臘蠟廊朗浪狼郎來冷勞擄櫓爐盧老蘆虜路露魯鷺碌祿綠菉錄鹿論壟弄籠聾牢磊賂雷壘屢樓淚漏累縷陋勒肋凜凌稜綾菱陵讀拏樂諾丹寧怒率異北磻便復不泌數索參塞省葉說殺辰沈拾若掠略亮兩凉梁糧良諒量勵呂女廬旅濾礪閭驪麗黎力曆歷轢年憐戀撚漣"] = "ꙂꙄꙆꙈꙊꙋꙌꙎꙐꙒꙔꙖꙘꙚꙜꙞꙠꙢꙤꙦꙨꙪꙬꚀꚂꚄꚆꚈꚊꚌꚎꚐꚒꚔꚖꚘꚚꜢꜤꜦꜨꜪꜬꜮꜲꜴꜶꜸꜺꜼꜾꝀꝂꝄꝆꝈꝊꝌꝎꝐꝒꝔꝖꝘꝚꝜꝞꝠꝢꝤꝦꝨꝪꝬꝮꝹꝻꝽꝾꞀꞂꞄꞆꞋꞍꞐꞒꞖꞘꞚꞜꞞꞠꞢꞤꞦꞨꞪꞫꞬꞭꞮꞰꞱꞲꞳꞴꞶ豈更車賈滑串句龜龜契金喇奈懶癩羅蘿螺裸邏樂洛烙珞落酪駱亂卵欄爛蘭鸞嵐濫藍襤拉臘蠟廊朗浪狼郎來冷勞擄櫓爐盧老蘆虜路露魯鷺碌祿綠菉錄鹿論壟弄籠聾牢磊賂雷壘屢樓淚漏累縷陋勒肋凜凌稜綾菱陵讀拏樂諾丹寧怒率異北磻便復不泌數索參塞省葉說殺辰沈拾若掠略亮兩凉梁糧良諒量勵呂女廬旅濾礪閭驪麗黎力曆歷轢年憐戀撚漣";
        lower_upper_map ["煉璉秊練聯輦蓮連鍊列劣咽烈裂說廉念捻殮簾獵令囹寧嶺怜玲瑩羚聆鈴零靈領例禮醴隸惡了僚寮尿料樂燎療蓼遼龍暈阮劉杻柳流溜琉留硫紐類六戮陸倫崙淪輪律慄栗率隆利吏履易李梨泥理痢罹裏裡里離匿溺吝燐璘藺隣鱗麟林淋臨立笠粒狀炙識什茶刺切度拓糖宅洞暴輻行降見廓兀嗀塚晴凞猪益礼神祥福靖精羽蘒諸逸都飯飼館鶴郞隷侮僧免勉勤卑喝嘆器塀墨層屮悔慨憎懲敏既暑梅海渚漢煮爫琢碑社祉祈祐祖祝禍禎穀突節練縉繁署者臭艹艹著褐視謁謹賓贈辶逸難響頻恵舘並况全侀充冀勇勺喝啕喙嗢塚墳奄奔婢嬨廒廙彩徭惘慎愈憎慠懲戴揄搜摒敖晴朗望杖歹殺流滛滋漢瀞煮瞧爵犯猪"] = "煉璉秊練聯輦蓮連鍊列劣咽烈裂說廉念捻殮簾獵令囹寧嶺怜玲瑩羚聆鈴零靈領例禮醴隸惡了僚寮尿料樂燎療蓼遼龍暈阮劉杻柳流溜琉留硫紐類六戮陸倫崙淪輪律慄栗率隆利吏履易李梨泥理痢罹裏裡里離匿溺吝燐璘藺隣鱗麟林淋臨立笠粒狀炙識什茶刺切度拓糖宅洞暴輻行降見廓兀嗀塚晴凞猪益礼神祥福靖精羽蘒諸逸都飯飼館鶴郞隷侮僧免勉勤卑喝嘆器塀墨層屮悔慨憎懲敏既暑梅海渚漢煮爫琢碑社祉祈祐祖祝禍禎穀突節練縉繁署者臭艹艹著褐視謁謹賓贈辶逸難響頻恵舘並况全侀充冀勇勺喝啕喙嗢塚墳奄奔婢嬨廒廙彩徭惘慎愈憎慠懲戴揄搜摒敖晴朗望杖歹殺流滛滋漢瀞煮瞧爵犯猪";
    }
    COT_Dir_T(volume, dik, "/utf8_names");
    #ifdef CREATION
        for (auto const& x : lower_upper_map) {
            COT_FileContent_T(volume, dik, "/utf8_names/" + x.first, x.first);
        }
    #else
        for (auto const& x : lower_upper_map) {
            COT_FileContent_T(volume, dik, "/utf8_names/" + x.second, x.first);
        }
    #endif
	
    // few random files with utf8 chars.
    COT_File_T(volume, dik, "/utf8_names/pomme.txt");
    COT_File_T(volume, dik, "/utf8_names/൧൨൩൪൫൬൭൮.txt");
    COT_File_T(volume, dik, "/utf8_names/éàùœ°æÂƒÌÚ®.txt");
    COT_File_T(volume, dik, "/utf8_names/ﰨﰩﰪﰫﰬﰭﰮﰯﰰﰱﰲﰳ.txt");
    COT_File_T(volume, dik, "/utf8_names/ォオカガキギクグケゲ.txt");
    COT_File_T(volume, dik, "/utf8_names/ꇆꇇꇈꇉꇊꇋꇌꇍ.txt");
    COT_File_T(volume, dik, "/utf8_names/伻似伽伾伿佀佁佂佃佄.txt");
    COT_File_T(volume, dik, "/utf8_names/楔楕楖楗楘楙楚楛楜楝楞楟楠楡.txt");

	// Normalisation equivalent
    #ifdef CREATION
	    COT_FileContent_T(volume, dik, "/utf8_names/\u03D3", "unicode u03D3 (same as u03D2u0301)");
	    COT_FileContent_T(volume, dik, "/utf8_names/\u038E", "unicode u038E (same as u03A5u0301)");
	#else
	    COT_FileContent_T(volume, dik, "/utf8_names/\xCF\x93", "unicode u03D3 (same as u03D2u0301)");
	    //COT_FileContent_T(volume, dik, "/utf8_names/\u03D3", "unicode u03D3 (same as u03D2u0301)"); //Visual studio cannot cope with \u03D3. It complained about code page 1252 something... Using UTF8 representation; converted by https://r12a.github.io/app-conversion/
	    COT_FileContent_T(volume, dik, "/utf8_names/\xCF\x92\xCC\x81", "unicode u03D3 (same as u03D2u0301)");
	    //COT_FileContent_T(volume, dik, "/utf8_names/\u03D2\u0301", "unicode u03D3 (same as u03D2u0301)");
	    COT_FileContent_T(volume, dik, "/utf8_names/\xCE\x8E", "unicode u038E (same as u03A5u0301)");
	    //COT_FileContent_T(volume, dik, "/utf8_names/\u038E", "unicode u038E (same as u03A5u0301)");
	    COT_FileContent_T(volume, dik, "/utf8_names/\xCE\xA5\xCC\x81", "unicode u038E (same as u03A5u0301)");
	   //COT_FileContent_T(volume, dik, "/utf8_names/\u03A5\u0301", "unicode u038E (same as u03A5u0301)");
#endif

    // folder
    if ( size == DISKIMAGE_SIZE_BIG ){
        string currentfolder = "/folder+lotsoffiles";
        COT_Dir_T(volume, dik, currentfolder);
        for (int i=0 ; i< (size==DISKIMAGE_SIZE_SMALL ? 5 : 5) ; i++) {
            COT_File_T(volume, dik, stringPrintf("%s/file_%d", currentfolder.c_str(), i), true);
            string subfolder1 = COT_Dir_T(volume, dik, stringPrintf("%s/folder_%d", currentfolder.c_str(), i));
            for (int j=0 ; j< (size==DISKIMAGE_SIZE_SMALL ? 7 : 70) ; j++) {
                COT_File_T(volume, dik, stringPrintf("%s/file_%d_%d", subfolder1.c_str(), i, j));
                string subfolder2 = COT_Dir_T(volume, dik, stringPrintf("%s/folder_%d_%d", subfolder1.c_str(), i, j));
                for (int k=0 ; k< (size==DISKIMAGE_SIZE_SMALL ? 9 : 90) ; k++) {
                    COT_File_T(volume, dik, stringPrintf("%s/file_%d_%d_%d", subfolder2.c_str(), i, j, k));
                    COT_Dir_T(volume, dik, stringPrintf("%s/empty_folder_%d_%d_%d", subfolder2.c_str(), i, j ,k));
                }
            }
        }
    }
    if ( size == DISKIMAGE_SIZE_BIG ) {
        string currentfile = "/file+4g5";
        _fillFile(volume, currentfile, dik, 4500000000);
    }

#endif // !DEBUG

}

