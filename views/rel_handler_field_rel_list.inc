<?php

class rel_handler_field_rel_list extends views_handler_field {
  function construct() {
    parent::construct();

  $this->additional_fields['list_id'] = array(
    'table' => 'rel_list_entity',
    'field' => 'list_id',
  );
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
  return $values->rel_list_entity_list_id;
  }
}
