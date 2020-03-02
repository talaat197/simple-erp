<?php
/**********************************************************************
    
***********************************************************************/
//----------------------------------------------------------------------------------
//	AgroPhos global system names. They have to be stored separate from
//	sys types defines to enable inclusion after every language change
//	(which happens for multilanguage bulk reports)
//
global $systypes_array, $bank_account_types, $bank_transfer_types, 
	$payment_person_types, $wo_types_array, $wo_cost_types, $class_types,
	$quick_actions, $quick_entry_types, $stock_types, $tag_types, $bank_owner;

$systypes_array = array (
	ST_JOURNAL => _("Journal Entry"),
	ST_BANKPAYMENT => _("Bank Payment"),
	ST_BANKDEPOSIT => _("Bank Deposit"),
	ST_BANKTRANSFER => _("Funds Transfer"),
	ST_SALESINVOICE => _("Sales Invoice"),
	ST_CUSTCREDIT => _("Customer Credit Note"),
	ST_CUSTPAYMENT => _("Customer Payment"),
	ST_CUSTDELIVERY => _("Delivery Note"),
	ST_LOCTRANSFER => _("Location Transfer"),
	ST_INVADJUST => _("Inventory Adjustment"),
	ST_PURCHORDER => _("Purchase Order"),
	ST_SUPPINVOICE => _("Supplier Invoice"),
	ST_SUPPCREDIT => _("Supplier Credit Note"),
	ST_SUPPAYMENT => _("Supplier Payment"),
	ST_SUPPRECEIVE => _("Purchase Order Delivery"),
	ST_WORKORDER => _("Work Order"),
	ST_MANUISSUE => _("Work Order Issue"),
	ST_MANURECEIVE => _("Work Order Production"),
	ST_SALESORDER => _("Sales Order"),
	ST_SALESQUOTE => _("Sales Quotation"),
	ST_COSTUPDATE => _("Cost Update"),
	ST_DIMENSION => _("Dimension"),
	ST_CUSTOMER => _("Customer"),
	ST_SUPPLIER => _("Supplier"),
	);

$fa_systypes_array = array (
	ST_INVADJUST => _("Fixed Assets Disposal"),
	ST_COSTUPDATE => _("Fixed Assets Revaluation"),
	);

$type_shortcuts = array(
	ST_JOURNAL => _("GJ"), 
	ST_BANKPAYMENT => _("BP"),
	ST_BANKDEPOSIT => _("BD"),
	ST_BANKTRANSFER => _("BT"),
	ST_SALESINVOICE => _("SI"),
	ST_CUSTCREDIT => _("CN"),
	ST_CUSTPAYMENT => _("CP"),
	ST_CUSTDELIVERY => _("DN"),
	ST_LOCTRANSFER => _("IT"), 
	ST_INVADJUST => _("IA"),
	ST_PURCHORDER => _("PO"),
	ST_SUPPINVOICE => _("PI"), 
	ST_SUPPCREDIT => _("PC"),
	ST_SUPPAYMENT => _("SP"),
	ST_SUPPRECEIVE => _("GRN"),
	ST_WORKORDER => _("WO"),
	ST_MANUISSUE => _("WI"),
	ST_MANURECEIVE => _("WP"),
	ST_SALESORDER => _("SO"),
	ST_SALESQUOTE => _("SQ"),
	ST_COSTUPDATE => _("CU"),
	ST_DIMENSION => _("Dim")
);


//----------------------------------------------------------------------------------
//		Bank transaction types
//
$bank_account_types = array (
	BT_TRANSFER => _("Savings Account"),
		_("Chequing Account"),
		_("Credit Account"),
		_("Cash Account")
	);

$bank_transfer_types = array(
	BT_TRANSFER => _("Transfer"),
			_("Cheque"),
			_("Credit"),
			_("Cash")
	);

//----------------------------------------------------------------------------------
//	Payment types
//

$payment_person_types = array (
	PT_MISC => _("Miscellaneous"),
				_("Work Order"),
				_("Customer"),
				_("Supplier"),
				_("Quick Entry")
	);

//----------------------------------------------------------------------------------
//	Manufacturing types
//
$wo_types_array = array (
	WO_ASSEMBLY => _("Assemble"),
	WO_UNASSEMBLY => _("Unassemble"),
	WO_ADVANCED => _("Advanced Manufacture")
	);

$wo_cost_types = array(
	WO_LABOUR => _("Labour Cost"),
	WO_OVERHEAD => _("Overhead Cost"),
	WO_MATERIALS => _("Materials")
);

//----------------------------------------------------------------------------------
//	GL account classes
//
$class_types = array(
	CL_ASSETS => _("Assets"),
	CL_LIABILITIES => _("Liabilities"),
	CL_EQUITY => _("Equity"),
	CL_INCOME => _("Income"),
	CL_COGS => _("Cost of Goods Sold"),
	CL_EXPENSE => _("Expense"),
);

//----------------------------------------------------------------------------------
//	Quick entry types
//
$quick_actions = array(
	'=' => _('Remainder'), 
	'a' => _('Amount'), 
	'a+' => _('Amount, increase base'), 
	'a-' => _('Amount, reduce base'), 
	'%' => _('% amount of base'),	
	'%+' => _('% amount of base, increase base'),	
	'%-' => _('% amount of base, reduce base'),	
	'T' => _('Taxes added'), 
	'T+' => _('Taxes added, increase base'), 
	'T-' => _('Taxes added, reduce base'), 
	't' => _('Taxes included'), 
	't+' => _('Taxes included, increase base'), 
	't-' => _('Taxes included, reduce base') 
);

$quick_entry_types = array(
	QE_DEPOSIT => _("Bank Deposit"),
	QE_PAYMENT => _("Bank Payment"),
	QE_JOURNAL => _("Journal Entry"),
	QE_SUPPINV => _("Supplier Invoice/Credit")
);

//----------------------------------------------------------------------------------

$depreciation_methods = array(
  'D' => _("Declining balance"),
  'S' => _("Straight line"),
  'N' => _("Sum of the Year Digits"),
  'O' => _("One-time"),
);

//----------------------------------------------------------------------------------

$stock_types = array(
	'M' => _("Manufactured"),
	'B' => _("Purchased"),
	'D' => _("Service")
);

//----------------------------------------------------------------------------------

$tag_types = array (
	TAG_ACCOUNT   => _("Account"),
	TAG_DIMENSION => _("Dimension")
);

//----------------------------------------------------------------------------------

$sys_crm_cats = array (
	'cust_branch' => _("Customer branch"),
	'supplier' => _("Supplier"),
	'shipper' => _("Shipper"),
	'company' => _("Company internal")
);
//----------------------------------------------------------------------------------

$pterm_types = array(
	PTT_PRE => _("Prepayment"),
	PTT_CASH => _("Cash"),
	PTT_DAYS => _("After No. of Days"),
	PTT_FOLLOWING => _("Day In Following Month")
);

$tax_algorithms = array( 
	TCA_LINES => _("Sum per line taxes"), 
	TCA_TOTALS => _("Taxes from totals")
);
//----------------------------------------------------------------------------------

$bank_owner_types = array(
	BO_UNKNOWN => _("Unknown"),
	BO_COMPANY => _("Company"),
	BO_CUSTBRANCH => _("Customer"),
	BO_SUPPLIER => _("Supplier")
);


$tmonths = array("", _("Jan"),_("Feb"),_("Mar"),_("Apr"),_("May"),_("Jun"),_("Jul"),_("Aug"),_("Sep"),_("Oct"),_("Nov"),_("Dec"));

//
//	FA supported db backend language settings.
//
$supported_collations = array(
  'utf8_xx' => _('Unicode (multilanguage)'),
  'utf8_is' => _('Icelandic'),
  'utf8_lv' => _('Latvian'),
  'utf8_ro' => _('Romanian'),
  'utf8_sl' => _('Slovenian'),
  'utf8_pl' => _('Polish'),
  'utf8_et' => _('Estonian'),
  'utf8_es' => _('Spanish'), 
  'utf8_sw' => _('Swedish'),
  'utf8_tr' => _('Turkish'),
  'utf8_cs' => _('Czech'),
  'utf8_da' => _('Danish'),
  'utf8_lt' => _('Lithuanian'),
  'utf8_sk' => _('Slovak'),
  'utf8_sp' => _('Spanish (alternative)'),
  'utf8_fa' => _('Persian'),
  'utf8_hu' => _('Hungarian'),
  'utf8_fr' => _('French'),
  'utf8_it' => _('Italian'),
);

