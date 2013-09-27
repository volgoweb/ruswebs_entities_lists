<?php
  
function rel_add_entity_to_list_checkbox(&$form, &$submit_array, $list_id, $entity_type, $entity_id = NULL, $weight = 1000) {
  $list = new rel_list($list_id);

  $list_title = $list->get_title();
  dsm($list_title);
  // return;

  $name = "rel_entity_to_list_$list_id";
  $form[$name] = array(
    '#type' => 'checkbox',
    '#title' => t('Add to list "@title"', array('@title' => $list_title)),
    '#default_values' => !empty($entity_id) ? $list->is_entity_in_list($entity_id) : 0,
  );

  if (!isset($form['rel_entity_type'])) {
    $form['rel_entity_type'] = array(
      '#type' => 'hidden',
      '#value' => $entity_type,
    );
  }

  $submit_array[] = 'rel_add_entity_to_list_submit';
}

function rel_add_entity_to_list_submit(&$form, &$form_state) {
  // $entity_id = $form_state['values']
  dsm($form);
  dsm($form_state);
  $entity_type = $form_state['values']['rel_entity_type'];
  $entity = $form_state[$entity_type];

  if (!empty($form_state['values'])) {
    foreach ($form_state['values'] as $key => $value) {
      if (strpos($key, 'rel_entity_to_list_') !== FALSE) {
        $list_id = str_replace('rel_entity_to_list_', '', $key);
        $list_id = (int) $list_id;
        $list = new rel_list($list_id);
        $entity_id = rel_list::get_entity_id($entity, $entity_type);
        if (!empty($entity_id) && !empty($entity_type)) {
          if (empty($form_state['values'][$key])) {
            $list->remove_entity($entity_id, $entity_type);
          }
          else {
            $list->add_entity($entity_id, $entity_type);
          }
        }
      }
    }
  }
}
