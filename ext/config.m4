dnl config.m4 for colopl_bc extension

PHP_ARG_ENABLE(colopl_bc, whether to enable colopl_bc backwards compatibility support,
[  --enable-colopl_bc           Enable colopl_bc support])

if test "$PHP_COLOPL_BC" != "no"; then
  PHP_NEW_EXTENSION(colopl_bc, colopl_bc_php70.c colopl_bc_php74.c colopl_bc.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
