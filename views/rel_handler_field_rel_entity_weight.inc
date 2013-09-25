<?php

class rel_handler_field_rel_entity_weight extends views_handler_field {
  function construct() {
    parent::construct();

	$this->additional_fields['weight'] = array(
		'table' => 'rel_list_entity',
		'field' => 'weight',
	);
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
	return $values->rel_list_entity_weight;
  }
}
