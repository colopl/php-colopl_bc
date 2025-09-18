--TEST--
Check for \Colopl\ColoplBc\Php70\str_shuffle() function compatibility. (ZTS)
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
--SKIPIF--
<?php
if ((bool) PHP_ZTS === false) print 'skip NTS not supported';
if (PHP_OS_FAMILY === 'Windows') print 'skip Windows not supported';
?>
--FILE--
<?php
const SEED = 1234;
\Colopl\ColoplBc\Php70\srand(\SEED);
echo \Colopl\ColoplBc\Php70\str_shuffle('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.');
?>
--EXPECT--
ogiuotram sciodr tc uu rixiepeo  dtloduornm ecrndto t posn iiqeaarv iot ioitchuteu emtod tnceg tuaemiiq mrcreeu lllmufooonliease rcnieoirtrcsdt  tlsor ncmitntae atu dmrmtpdieu auvn,po en lmotetinsrEasn euq s e aiaormpiqasmse oit axtcsoib. d . tasca  pns clmtbc aoum luid ditaea e iuerioetiia  sinueu nuuodapal lgrnmun ntu ,od,pmsetpitlt aolreed rdauf ori ,be  exel nd mitcliaiie  lDsn cvre Uinuoiinleeadqp  i.aucsidulfullL t apie nn oiseu.r laai
