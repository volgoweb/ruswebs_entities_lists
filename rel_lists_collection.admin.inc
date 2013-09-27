<?php
/**
 * Административный интерфейс управления коллекцией списков
 */

function rel_lists_collection_page() {
  $collection = new rel_lists_collection();
  $raw_lists = $collection->get_raw();

  $rows = array();
  if (!empty($raw_lists)) {
    foreach ($raw_lists as $i => $raw) {
      $list_href     = sprintf('%s/list/%d', REL_URL_ADMIN_LISTS, $raw->id);
      $entities_href = sprintf('%s/list/%d/entities', REL_URL_ADMIN_LISTS, $raw->id);
      $rows[] = array(
        'data' => array(
          l($raw->title, $list_href),
          l(t('Entities of list'), $entities_href),
        ),
      );
    }
  }

  $header = array(
    t('Title'),
    t('Entities of list'),
  );

  $button = l(t('Add'), REL_URL_ADMIN_ADD_LIST, array(
    'attributes' => array(
      'class' => array(
        'button',
      ),
    ),
  ));

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
  )) . $button;
}

