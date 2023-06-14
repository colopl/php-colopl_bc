--TEST--
Check for \Colopl\ColoplBc\Php70\str_shuffle() function compatibility.
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php
if (PHP_ZTS !== 0) print 'skip ZTS not supported'; 
?>
--FILE--
<?php
const SEED = 1234;
\Colopl\ColoplBc\Php70\srand(\SEED);
echo \Colopl\ColoplBc\Php70\str_shuffle('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.');
?>
--EXPECT--
cfapiqi uogi txnrdaeqvnataten ume v aiisalm  isiatdaduaeudr eenteotscdm itdenrieuaicii loi aaot imxll ne luecat uri ipp a l moplaur tsarc.eptuisrccdlotep ecorU oeaceqsuerno .t li sa  rerldulidieoor i n nd  tloiii eeulds  ithsnot btrnf ilm a,nuto umdscd viee.amndrueonmce ns dancratmamo inuios ilmo sg a neo ci. eispiisu pueoe umurt denaamlmiu ,loi ptctaurst lrinm usLrdto  gru coaetucnuliittro, npemeDnpt s,efdlq q teoiabuEet ert ioinu tlbxoo o 
