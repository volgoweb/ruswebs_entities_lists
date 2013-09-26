<?php
/**
 * Implements of hook_views_data().
 */
function ruswebs_entities_lists_views_data() {
  $module_path = drupal_get_path('module', 'ruswebs_entities_lists');
  require_once($module_path . '/rel_list.class.inc');

  $data['rel_list_entity']['table']['group'] = t('Entity');
  $entities_info = entity_get_info();
  foreach ($entities_info as $type => $params) {
    $id_col = rel_list::get_entity_id_colname($type);
    $data['rel_list_entity']['table']['join'][$type] = array(
      'left_field' => $id_col,
      'field' => 'entity_id',
      'extra' => array(
        array(
          'field' => 'entity_type', 
          'value' => $type,
        ),
      ),
    );
  }

  $data['rel_list_entity']['list_id'] = array(
    'title' => t('rel list'),
    'help' => t('Provide a filter entities by list id.'),
    'field' => array(
      'handler' => 'rel_handler_field_rel_list',
    ),
    'filter' => array(
      'handler' => 'rel_handler_filter_rel_list'
    ),
  );

  $data['rel_list_entity']['weight'] = array(
    'title' => t('Entity weight in list'),
    'help' => t('Entity weight in list.'),
    'field' => array(
      'handler' => 'rel_handler_field_rel_entity_weight',
    ),
    'sort' => array(
      'handler' => 'rel_handler_sort_rel_entity_weight'
    ),
  );

  return $data;
}

/**
* Implementation of hook_views_handlers().
*/
function ruswebs_entities_lists_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'ruswebs_entities_lists'),
    ),
    'handlers' => array(
    'rel_handler_field_rel_list' => array(
      'parent' => 'views_handler_field',
    ),
    'rel_handler_filter_rel_list' => array(
      'parent' => 'views_handler_filter',
    ),
    'rel_handler_field_rel_entity_weight' => array(
      'parent' => 'views_handler_field',
    ),
    'rel_handler_sort_rel_entity_weight' => array(
      'parent' => 'views_handler_sort',
    ),
    ),
  );
}
