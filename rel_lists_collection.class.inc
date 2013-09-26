<?php
  
/**
 * Класс, описывающий коллекцию списков сущностей
 */
class rel_lists_collection {
  const LIST_TBL = 'rel_list';

  public function add($lists) {
    if (!in_array($lists)) {
      $lists = array($lists);
    }
    if (empty($lists)) {
      return;
    }

    // существующие списки
    $existing_ids = $this->get_ids();

    // список полей для обновления или вставки в таблицу БД
    $fields = array(
      'title',
      'max_size',
      'autoadds',
      'entity_params',
    );

    $insert_query = db_insert(self::LIST_TBL, 't')
      ->fields($fields);

    $update_query = db_update(self::LIST_TBL, 't')
      ->fields($fields);

    foreach ($lists as $key => $value) {
      $fields = array(
        'title'        => $value->title,
        'max_size'     => $value->max_size,
        'autoadds'     => $value->autoadds,
        'entity_params' => serialize($value->entity_params),
      );

      if (!empty($value->id) && in_array($value->id, $existing_ids)) {
        // Существующий список помещаем в запрос обновления
        $update_query->values($fields);
        continue;
      }
      else {
        // Новый список помещаем в запрос добавления
        $insert_query->values($fields);
      }
    }
  }

  public function get_raw() {
    $raw = db_select(self::LIST_TBL, 't')
      ->fields('t')
      ->execute()
      ->fetchAll();
    
	if (empty($raw)) {
		return array();
	}
    
    return $raw;
  }

  public function get_all() {
    $collection = array();

    $raw = $this->get_raw();
    if ($raw) {
      foreach ($raw as $key => $row) {
        $list = new rel_list();
        $collection[] = $list;  
      }
    }    
    return $collection;
  }

  public function get_ids() {
    $ids = db_select(self::LIST_TBL, 't')
      ->fields('t', array('id'))
      ->execute()
      ->fetchCol();
	if ($ids) {
		return $ids;
	}
	else {
		return array();
	}
  }

  /**
   * Возвращает массив тех списков, которые могут содержать указанный тип (и bundle) сущности
   * с включенным автодобавлением в список.
   * @params string $type - тип сущности
   * @params string $bundle - bundle сущности
   */
  public function get_lists_with_autoadds($type, $bundle) {
    $key_entity_type = rel_list::get_key_entity_type($type, $bundle);
    $search = sprintf('s:%d:"%s"', strlen($key_entity_type), $key_entity_type);
    $text = '%' . db_like($search) . '%';

    $raws = db_select(self::LIST_TBL, 't')
      ->fields('t')
      ->condition('t.entity_params', $text, 'LIKE')
      ->condition('t.entity_params', '%"autoadds";i:1%', 'LIKE')
      ->execute()
      ->fetchAll();
    
	if (empty($raws)) {
		return array();
	}

    $lists = array();
    foreach ($raws as $i => $obj) {
      $params = array(
        'title' => $obj->title,
        'max_size' => $obj->max_size,
        'entity_params' => unserialize($obj->entity_params),
      );
      $lists[] = new rel_list($obj->id, $params);
    }

    return $lists;
  }
}
