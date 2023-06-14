<?php

declare(strict_types=0);

namespace Colopl\ColoplBc\Tests\Extension;

use Colopl\ColoplBc\Tests\AbstractTestCase;

final class PHP70Test extends AbstractTestCase
{
    protected function setUp(): void
    {
        parent::setUp();

        if (!\extension_loaded('colopl_bc')) {
            self::markTestSkipped('colopl_bc extension is not loaded');
        }
    }

    public function testSRand(): void
    {
        \Colopl\ColoplBc\Php70\srand(1234);

        if (\PHP_ZTS === 0) {
            self::assertSame(\Colopl\ColoplBc\Php70\rand(), 479142414);
        } else {
            self::assertSame(\Colopl\ColoplBc\Php70\rand(), 1584308507);
        }
    }

    public function testSRandNull(): void
    {
        $seed = \null;

        if (\PHP_VERSION_ID >= 80100) {
            /* PHP 8.1 does not support null -> 0 auto casting. */
            $seed = (int) $seed;
        }

        /**
         * @phpstan-ignore-next-line
         * @psalm-suppress UnusedFunctionCall
         * @psalm-suppress PossiblyNullArgument
         */
        \Colopl\ColoplBc\Php70\srand($seed);
        if (\PHP_ZTS === 0) {
            self::assertSame(\Colopl\ColoplBc\Php70\rand(), 1804289383);
        } else {
            self::assertSame(\Colopl\ColoplBc\Php70\rand(), 1012484);
        }
    }

    public function testRand(): void
    {
        \Colopl\ColoplBc\Php70\srand(5678);
        if (\PHP_ZTS === 0) {
            self::assertSame(\Colopl\ColoplBc\Php70\rand(), 1860157456);
        } else {
            self::assertSame(\Colopl\ColoplBc\Php70\rand(), 1173971923);
        }
    }

    public function testGetRandMax(): void
    {
        self::assertSame(\Colopl\ColoplBc\Php70\getrandmax(), 2147483647);
    }

    public function testShuffle(): void
    {
        $arr = [1, 2, 3, 4];
        \Colopl\ColoplBc\Php70\srand(1234);
        \Colopl\ColoplBc\Php70\shuffle($arr);

        if (\PHP_ZTS === 0) {
            self::assertSame($arr, [2, 3, 4, 1]);
        } else {
            self::assertSame($arr, [4, 2, 1, 3]);
        }
    }

    public function testStrShuffle(): void
    {
        $str = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.';
        \Colopl\ColoplBc\Php70\srand(1234);

        if (\PHP_ZTS === 0) {
            self::assertSame(
                \Colopl\ColoplBc\Php70\str_shuffle($str),
                'cfapiqi uogi txnrdaeqvnataten ume v aiisalm  isiatdaduaeudr eenteotscdm itdenrieuaicii loi aaot imxll ne luecat uri ipp a l moplaur tsarc.eptuisrccdlotep ecorU oeaceqsuerno .t li sa  rerldulidieoor i n nd  tloiii eeulds  ithsnot btrnf ilm a,nuto umdscd viee.amndrueonmce ns dancratmamo inuios ilmo sg a neo ci. eispiisu pueoe umurt denaamlmiu ,loi ptctaurst lrinm usLrdto  gru coaetucnuliittro, npemeDnpt s,efdlq q teoiabuEet ert ioinu tlbxoo o '
            );
        } else {
            self::assertSame(
                \Colopl\ColoplBc\Php70\str_shuffle($str),
                'ogiuotram sciodr tc uu rixiepeo  dtloduornm ecrndto t posn iiqeaarv iot ioitchuteu emtod tnceg tuaemiiq mrcreeu lllmufooonliease rcnieoirtrcsdt  tlsor ncmitntae atu dmrmtpdieu auvn,po en lmotetinsrEasn euq s e aiaormpiqasmse oit axtcsoib. d . tasca  pns clmtbc aoum luid ditaea e iuerioetiia  sinueu nuuodapal lgrnmun ntu ,od,pmsetpitlt aolreed rdauf ori ,be  exel nd mitcliaiie  lDsn cvre Uinuoiinleeadqp  i.aucsidulfullL t apie nn oiseu.r laai'
            );
        }
    }

    public function testArrayRand(): void
    {
        \Colopl\ColoplBc\Php70\srand(1234);

        if (\PHP_ZTS === 0) {
            self::assertSame(
                \Colopl\ColoplBc\Php70\array_rand([1, 2, 3, 4, 5], 3),
                [0, 1, 3]
            );
        } else {
            self::assertSame(
                \Colopl\ColoplBc\Php70\array_rand([1, 2, 3, 4, 5], 3),
                [1, 3, 4]
            );
        }
    }

    public function testMtSRand(): void
    {
        \Colopl\ColoplBc\Php70\mt_srand(1234);
        self::assertSame(\Colopl\ColoplBc\Php70\mt_rand(), 1741177057);
    }

    public function testMtSRandNull(): void
    {
        $seed = \null;

        if (\PHP_VERSION_ID >= 80100) {
            /* PHP 8.1 does not support null -> 0 auto casting. */
            $seed = (int) $seed;
        }

        /**
         * @phpstan-ignore-next-line
         * @psalm-suppress PossiblyNullArgument
         */
        \Colopl\ColoplBc\Php70\mt_srand($seed);
        self::assertSame(
            \Colopl\ColoplBc\Php70\mt_rand(),
            963932192
        );
    }

    public function testMtRand(): void
    {
        \Colopl\ColoplBc\Php70\mt_srand(1234);
        self::assertSame(\Colopl\ColoplBc\Php70\mt_rand(), 1741177057);
    }

    public function testMtGetRandMax(): void
    {
        self::assertSame(\Colopl\ColoplBc\Php70\mt_getrandmax(), 2147483647);
    }

    public function testDateCreate(): void
    {
        $dates = [];
        foreach (\range(1, 100) as $_i) {
            $dates[] = \Colopl\ColoplBc\Php70\date_create();
            usleep(10);
        }
        foreach ($dates as $date) {
            self::assertInstanceOf(\DateTime::class, $date);
            /**
             * @phpstan-ignore-next-line
             * @psalm-suppress RedundantConditionGivenDocblockType
             */
            self::assertSame($date instanceof \DateTime ? $date->format('u') : \null, '000000');
        }
    }

    public function testDateCreateImmutable(): void
    {
        $dates = [];
        foreach (\range(1, 100) as $_i) {
            $dates[] = \Colopl\ColoplBc\Php70\date_create_immutable();
            usleep(10);
        }
        foreach ($dates as $date) {
            self::assertInstanceOf(\DateTimeImmutable::class, $date);
            /**
             * @phpstan-ignore-next-line
             * @psalm-suppress RedundantConditionGivenDocblockType
             */
            self::assertSame($date instanceof \DateTimeImmutable ? $date->format('u') : \null, '000000');
        }
    }

    public function testDateCreateFailure(): void
    {
        self::assertFalse(\Colopl\ColoplBc\Php70\date_create('bongo'));
    }

    public function testDateCreateImmutableFailure(): void
    {
        self::assertFalse(\Colopl\ColoplBc\Php70\date_create_immutable('conga'));
    }
}
