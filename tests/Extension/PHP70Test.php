<?php

declare(strict_types=0);

namespace Colopl\ColoplBc\Tests\Extension;

use Colopl\ColoplBc\Tests\AbstractTestCase;

final class PHP70Test extends AbstractTestCase
{
    /**
     * @fixme Support ZTS
     */
    private static function markZtsNotSupported(): void
    {
        if (\PHP_ZTS === 1) {
            self::markTestSkipped('ZTS is not supported currently');
        }
    }

    protected function setUp(): void
    {
        parent::setUp();

        if (!\extension_loaded('colopl_bc')) {
            self::markTestSkipped('colopl_bc extension is not loaded');
        }
    }

    public function testSRand(): void
    {
        self::markZtsNotSupported();

        \Colopl\ColoplBc\Php70\srand(1234);
        self::assertSame(
            \Colopl\ColoplBc\Php70\rand(),
            479142414
        );
    }

    public function testSRandNull(): void
    {
        self::markZtsNotSupported();

        if (\PHP_VERSION_ID >= 80100) {
            self::markTestSkipped('PHP 8.1.0 does not support null argument.');
        }

        /**
         * @phpstan-ignore-next-line
         * @psalm-suppress UnusedFunctionCall
         */
        \Colopl\ColoplBc\Php70\srand(null);
        self::assertSame(
            \Colopl\ColoplBc\Php70\rand(),
            1804289383
        );
    }

    public function testRand(): void
    {
        self::markZtsNotSupported();

        \Colopl\ColoplBc\Php70\srand(5678);
        self::assertSame(
            \Colopl\ColoplBc\Php70\rand(),
            1860157456
        );
    }

    public function testGetRandMax(): void
    {
        self::assertSame(
            \Colopl\ColoplBc\Php70\getrandmax(),
            2147483647
        );
    }

    public function testShuffle(): void
    {
        self::markZtsNotSupported();

        $arr = [1, 2, 3, 4];
        \Colopl\ColoplBc\Php70\srand(1234);
        \Colopl\ColoplBc\Php70\shuffle($arr);

        self::assertSame(
            $arr,
            [2, 3, 4, 1]
        );
    }

    public function testStrShuffle(): void
    {
        self::markZtsNotSupported();

        $str = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.';
        \Colopl\ColoplBc\Php70\srand(1234);

        self::assertSame(
            \Colopl\ColoplBc\Php70\str_shuffle($str),
            'cfapiqi uogi txnrdaeqvnataten ume v aiisalm  isiatdaduaeudr eenteotscdm itdenrieuaicii loi aaot imxll ne luecat uri ipp a l moplaur tsarc.eptuisrccdlotep ecorU oeaceqsuerno .t li sa  rerldulidieoor i n nd  tloiii eeulds  ithsnot btrnf ilm a,nuto umdscd viee.amndrueonmce ns dancratmamo inuios ilmo sg a neo ci. eispiisu pueoe umurt denaamlmiu ,loi ptctaurst lrinm usLrdto  gru coaetucnuliittro, npemeDnpt s,efdlq q teoiabuEet ert ioinu tlbxoo o '
        );
    }

    public function testArrayRand(): void
    {
        self::markZtsNotSupported();

        \Colopl\ColoplBc\Php70\srand(1234);
        self::assertSame(
            \Colopl\ColoplBc\Php70\array_rand([1, 2, 3, 4, 5], 3),
            [0, 1, 3]
        );
    }

    public function testMtSRand(): void
    {
        self::markZtsNotSupported();

        \Colopl\ColoplBc\Php70\mt_srand(1234);
        self::assertSame(
            \Colopl\ColoplBc\Php70\mt_rand(),
            1741177057
        );
    }

    public function testMtSRandNull(): void
    {
        self::markZtsNotSupported();

        if (\PHP_VERSION_ID >= 80100) {
            self::markTestSkipped('PHP 8.1.0 does not support null argument.');
        }

        /** @phpstan-ignore-next-line */
        \Colopl\ColoplBc\Php70\mt_srand(null);
        self::assertSame(
            \Colopl\ColoplBc\Php70\mt_rand(),
            963932192
        );
    }

    public function testMtRand(): void
    {
        self::markZtsNotSupported();

        \Colopl\ColoplBc\Php70\mt_srand(1234);
        self::assertSame(
            \Colopl\ColoplBc\Php70\mt_rand(),
            1741177057
        );
    }

    public function testMtGetRandMax(): void
    {
        self::assertSame(
            \Colopl\ColoplBc\Php70\mt_getrandmax(),
            2147483647
        );
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
            self::assertSame($date instanceof \DateTime ? $date->format('u') : null, '000000');
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
            self::assertSame($date instanceof \DateTimeImmutable ? $date->format('u') : null, '000000');
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
