<?php
	
/**
 * Класс, описывающий коллекцию списков сущностей
 */
class rel_lists_collection {
	const LIST_TBL = 'rel_list';

	public function add($entities_lists) {
		if (!in_array($entities_lists)) {
			$entities_lists = array($entities_lists);
		}
		if (empty($entities_lists)) return;

		// существующие списки
		$existing_ids = $this->get_ids();

		// список полей для обновления или вставки в таблицу БД
		$fields = array(
			'title',
			'max_size',
			'autoadds',
			'title_field',
			'entity_types',
		);

		$insert_query = db_insert(self::LIST_TBL, 'c')
			->fields($fields);

		$update_query = db_update(self::LIST_TBL, 'c')
			->fields($fields);

		foreach ($entities_lists as $key => $value) {
			$fields = array(
				'title'        => $value->title,
				'max_size'     => $value->max_size,
				'autoadds'     => $value->autoadds,
				'title_field'  => $value->title_field,
				'entity_types' => serialize($value->entity_types),
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
		$raw = db_select(self::LIST_TBL, 'c')
			->fields('c')
			->execute()
			->fetchAll();
		
		if (empty($raw)) return array();
		
		return $raw;
	}

	private function get_all() {
		$collection = array();

		$raw = $this->get_raw();
		if ($raw) {
			foreach ($raw as $key => $row) {
				$entities_list = new rel_entities_list();
				$collection[] = $entities_list;	
			}
		}		
		return $collection;
	}

	private function get_ids() {
		$ids = db_select(self::LIST_TBL, 'c')
			->fields('c', array('id'))
			->execute()
			->fetchCol();
		if ($ids) return $ids;
		else return array();
	}

}
