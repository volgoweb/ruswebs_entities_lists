<?php
  
function rel_add_entity_to_list_checkbox(&$form, &$submit_array, $list_id, $entity_type, $entity_id = NULL, $weight = 1000) {
  $list = new rel_list($list_id);

  $list_title = $list->get_title();
  dsm($list_title);
  // return;

  $name = "entity_to_list_$list_id";
  $form[$name] = array(
    '#type' => 'checkbox',
    '#title' => t('Add to list "@title"', array('@title' => $list_title)),
    '#default_values' => !empty($entity_id) ? $list->is_entity_in_list($entity_id) : 0,
  );

  $submit_array[] = 'rel_add_entity_to_list_submit';
}

function rel_add_entity_to_list_submit(&$form, &$form_state) {
  // $entity_id = $form_state['values']
  dsm($form);
  dsm($form_state);
}
