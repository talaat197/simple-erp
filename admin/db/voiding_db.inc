<?php
/**********************************************************************
    
***********************************************************************/
include_once($path_to_root . "/sales/includes/sales_db.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");
include_once($path_to_root . "/inventory/includes/inventory_db.inc");
include_once($path_to_root . "/manufacturing/includes/manufacturing_db.inc");

function void_transaction($type, $type_no, $date_, $memo_)
{
	global $Refs;
	$void_entry = get_voided_entry($type, $type_no);

	if ($void_entry != null)
		return _('This transaction was already voided before.');

	switch ($type) {
		case ST_JOURNAL : 
			if (!exists_gl_trans($type, $type_no))
				return _('Selected transaction does not exists.');
			if ($wo = check_wo_costing($type, $type_no))
				return sprintf(_('This transaction cannot be voided because it is part of Work Order %s costs.'), $wo);
			void_journal_trans($type, $type_no);
			break;

		case ST_BANKDEPOSIT : 
		case ST_BANKTRANSFER : 
			if (!check_void_bank_trans($type, $type_no))
				return _('This transaction cannot be voided because the operation would decrease account balance below allowed limit in some point of account history.');
		case ST_BANKPAYMENT : 
			if (!exists_bank_trans($type, $type_no))
				return _('Selected transaction does not exists.');
			void_bank_trans($type, $type_no);
			break;

		case ST_CUSTPAYMENT : 
			if (!check_void_bank_trans($type, $type_no))
				return _('This transaction cannot be voided because the operation would decrease account balance below allowed limit in some point of account history.');
		case ST_SALESINVOICE : 
 			if (is_cust_invoice_credited($type_no))
				return _('This invoice cannot be voided because it was already credited.');
		case ST_CUSTCREDIT : 
		case ST_CUSTDELIVERY : 
			if (!exists_customer_trans($type, $type_no))
				return _('Selected transaction does not exists.');
			if ($type == ST_CUSTDELIVERY)	
			{
				$childs = get_sales_child_lines($type, $type_no, false); 
				if ($childs && db_num_rows($childs))
					return _('This delivery cannot be voided because it was already invoiced.');
			}
			post_void_customer_trans($type, $type_no);
			break;

		case ST_LOCTRANSFER : 
			if (get_stock_transfer_items($type_no) == null)
				return _('Selected transaction does not exists.');
			void_stock_transfer($type_no);
			break;

		case ST_INVADJUST : 
			if (get_stock_adjustment_items($type_no) == null)
				return _('Selected transaction does not exists.');
			void_stock_adjustment($type_no);
			break;

		case ST_PURCHORDER : 
				return _('This transaction type cannot be voided.');
			break;

		case ST_SUPPRECEIVE : 
			if (exists_grn_on_invoices($type_no))
				return _('This GRN cannot be voided because it was already invoiced.');
			if (!post_void_supp_trans($type, $type_no))
				return _('Error encountered when voiding transaction.');
			break;

		case ST_SUPPINVOICE : 
		case ST_SUPPCREDIT : 
		case ST_SUPPAYMENT : 
			if (!exists_supp_trans($type, $type_no))
				return _('Selected transaction does not exists.');
			if (!post_void_supp_trans($type, $type_no))
				return _('Error encountered when voiding transaction.');
			break;

		case ST_WORKORDER : 
			if (!get_work_order($type_no, true))
				return _('Selected transaction does not exists.');
			void_work_order($type_no);
			break;

		case ST_MANUISSUE : 
			if (!exists_work_order_issue($type_no))
				return _('Selected transaction does not exists.');
			if ($wo = check_void_wo_issue($type_no))
				return sprintf(_('Selected document cannot be voided because related Work Order %s is already closed.'), $wo);
			void_work_order_issue($type_no);
			break;

		case ST_MANURECEIVE : 
			if (!exists_work_order_produce($type_no))
				return _('Selected transaction does not exists.');
			if ($wo = check_void_wo_production($type_no))
				return sprintf(_('Selected document cannot be voided because related Work Order %s is already closed.'), $wo);
			void_work_order_produce($type_no);
			break;

		case ST_SALESORDER: 
		case ST_SALESQUOTE: 
			return _('This transaction type cannot be voided.');

		case ST_COSTUPDATE : 
			return _('This transaction type cannot be voided.');
			break;
	}

	
	add_audit_trail($type, $type_no, $date_, _("Voided.")."\n".$memo_);
	$Refs->restore_last($type, $type_no);
	add_voided_entry($type, $type_no, $date_, $memo_);
	return false;
}

//--------------------------------------------------------------------------------------------------

function get_voided_entry($type, $type_no)
{
	$sql = "SELECT * FROM ".TB_PREF."voided WHERE type=".db_escape($type)
		." AND id=".db_escape($type_no);

	$result = db_query($sql, "could not query voided transaction table");

	return db_fetch($result);
}

//--------------------------------------------------------------------------------------------------

function add_voided_entry($type, $type_no, $date_, $memo_)
{
	$date = date2sql($date_);
	$sql = "INSERT INTO ".TB_PREF."voided (type, id, date_, memo_)
		VALUES (".db_escape($type).", ".db_escape($type_no).", "
		.db_escape($date).", ".db_escape($memo_).")";

	db_query($sql, "could not add voided transaction entry");
}

//--------------------------------------------------------------------------------------------------

