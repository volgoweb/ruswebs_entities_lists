<?php

class rel_handler_filter_rel_list extends views_handler_filter_in_operator {
	function get_value_options() {
		// Получаем все списки
		$collection = new rel_lists_collection();
		$lists_raw = $collection->get_raw();
	
		// сформируем массив options для элемента формы
		$options = array();
		if (!empty($lists_raw)) {
			foreach ($lists_raw as $i => $obj) {
				$options[$obj->id] = $obj->title;
			}
		}

		// устанавливаем возможные значения для фильтра
		$this->value_options = $options;
		return $this->value_options;
	}
}
