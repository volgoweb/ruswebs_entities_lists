<?php
	
/**
 * Класс, описывающий коллекцию списков сущностей
 */
class rel_lists_collection {
	private $tbl = 'rel_lists';

	public function add($entities_lists) {
		if (!in_array($entities_lists)) {
			$entities_lists = array($entities_lists);
		}

		$existing_ids = $this->get_ids();

		// записываем основные параметры всех списков
		if ($entities_lists) {
			$insert_fields = array();
			$update_fields = array();

			foreach ($entities_lists as $key => $value) {
				$fields = array(
					'title' => $value->title,
					'max_size' => $value->max_size,
					'autoadds' => $value->autoadds,
					'title_field' => $value->title_field,
				);

				if (!empty($value->id)) {
					if (in_array($value->id, $existing_ids)) {
						$update_fields[] = $fields;
						continue;
					}
				}

				$insert_fields[] = $fields();
			}
		}

		$fields = array(
			'title',
			'max_size',
			'autoadds',
			'title_field',
		);

		// вставляем новые списки в таблицу
		if ($insert_fields) {
			$query = db_insert($this->tbl, 'c')
				->fields($fields);
			foreach ($insert_fields as $key => $values) {
				$query->values($values);
			}
			$query->execute();
		}

		// обновляем существующие списки в таблицу
		if ($insert_fields) {
			$query = db_update($this->tbl, 'c')
				->fields($fields);
			foreach ($insert_fields as $key => $values) {
				$query->values($values);
			}
			$query->execute();
		}
		
	}

	private function get_raw() {
		$raw = db_select($this->tbl, 'c')
			->fields('c')
			->execute()
			->fetchAll();
		
		if (empty($raw)) return array();
		
		return $raw;
	}

	private function get_collection() {
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
		$ids = db_select($this->tbl, 'c')
			->fields('c', array('id'))
			->execute()
			->fetchCol();
		if ($ids) return $ids;
		else return array();
	}

}
