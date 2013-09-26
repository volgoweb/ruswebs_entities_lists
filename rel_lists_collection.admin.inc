<?php
/**
 * Административный интерфейс управления коллекцией списков
 */

function rel_lists_collection_page() {
  $collection = new rel_lists_collection();
  $raw_lists = $collection->get_raw();
  return theme('lists_collection_page', array('lists' => $raw_lists));
}

