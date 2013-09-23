<?php
	
/**
 * Класс, описывающий список сущностей
 */
class rel_list {
	/**
	 * Индентификатор списка
	 */
	protected $id;
	/**
	 * Человекопонятное имя списка
	 */
	protected $title;
	/**
	 * Максимальное количество сущностей, допустимое в списке
	 */
	protected $max_size = 5;
	/**
	 * Массив, содержащий настройки списка по типам сущностей, которые могут присутствовать в списке.
	 */
	protected $entity_params = array();
	/**
	 * Массив, содержащий id, заголовок(title), вес(weight), сущностей из данного списка
	 */
	protected $entites  = array();
	/*
	 * здесь будем хранить массив описаний всех сущностей сайта
	 * Специально статическая переменная, так как массив описаний не зависит от конкретного списка,
	 * а зависит лишь от имеющихся сущностей на сайте.
	 * При первом же заполнении этой переменной мы можем ее использовать где угодно, 
	 * не вызывая лишний раз функцию получения этих данных
	 */
	protected static $entities_info;
	/**
	 * Имя таблицы, содержащей данные о списке (не включая данные о всех сущностях списка)
	 */
	const LIST_TBL = 'rel_list';
	/**
	 * Имя таблицы, содержащей данные о всех сущностях, добавленных в список
	 */
	const ENTITIES_TBL = 'rel_list_entity';

	public function __construct($id) {
		if (!empty($id)) {
			$this->id = (int)$id;
			$this->load();
		}
	}

	/**
	 * Устанавливает переданные значения для переменных экземпляара класса.
	 * @param array $params - массив значений
	 */
	public function set($params) {
		if (!empty($params['id'])) {
			$this->id = (int)$params['id'];
		}

		$this->title = $params['title'];

		$this->max_size = (int)$params['max_size'];

		$this->entity_params = $params['entity_params'];
	}

	/**
	 * Устанавливает значения из базы данных для переменныз экземпляара класса
	 */
	private function load() {
		$raw = db_select(self::LIST_TBL, 't')
			->fields('t')
			->condition('t.id', $this->id)
			->execute()
			->fetchObject();

		foreach (array('title', 'max_size') as $name) {
			$this->$name = $raw->$name;
		}

		$this->entity_params = unserialize($raw->entity_params);
	}

	/**
	 * @return int индентификатор списка
	 */
	public function get_id() {
		return $this->id;
	}

	/**
	 * @return string человекопонятное имя списка
	 */
	public function get_title() {
		return $this->title;
	}

	/**
	 * @return int максимальное количество сущностей, которое может быть в списке
	 */
	public function get_max_size() {
		return $this->max_size;
	}

	/**
	 * Сохраняет данные экземпляра класса в базе данных
	 */
	public function save() {
		if (!empty($this->id)) {
			$this->update();
		}
		else {
			$this->create();
		}
	}

	/**
	 * Обновляет данные экземпляра класса в базе данных
	 */
	private function update() {
		$fields = $this->get_fields_for_query();
		unset($fields['id']);

		db_update(self::LIST_TBL)
			->fields($fields)
			->condition('id', $this->id)
			->execute();
	}

	/**
	 * Добавляет данные экземпляра класса в базе данных
	 */
	private function create() {
		$fields = $this->get_fields_for_query();

		db_insert(self::LIST_TBL)
			->fields($fields)
			->execute();
	}

	private function get_fields_for_query() {
		$fields = array();
		foreach (array('title', 'max_size', 'entity_params') as $name) {
			$fields[$name] = $this->$name;
		}

		if (!empty($this->id)) {
			$fields['id'] = $this->id;
		}

		$fields['entity_params'] = serialize($fields['entity_params']);

		return $fields;
	}

	public function can_entity_add_to_list($type, $bundle) {
		$key = self::get_key_entity_type($type, $bundle);
		if (!empty($this->entity_params[$key])) {
			return TRUE;
		}
		return FALSE;
	}

	public function is_entity_autoadd_to_list($type, $bundle) {
		$key = self::get_key_entity_type($type, $bundle);
		if (!empty($this->entity_params[$key])) {
			if (!empty($this->entity_params[$key]['autoadds'])) {
				return TRUE;
			}
		}
		return FALSE;
	}

	public static function get_key_entity_type($type, $bundle) {
		return $type . '__' . $bundle;
	}

	public static function get_array_entity_params($type, $bundle, $autoadds) {
		$entity_params[ rel_list::get_key_entity_type($type, $bundle) ] = array(
			'autoadds' => $autoadds,
			'type'     => $type,
			'bundle'   => $bundle,
		);

		return $entity_params;
	}

	public function get_entities_array() {
		$raws = db_select(self::ENTITIES_TBL, 't')
			->fields('t')
			->condition('t.list_id', $this->id)
			->orderBy('t.weight', 'ASC')
			->execute()
			->fetchAll();

		$array = array(); 
		if ($raws) {
			foreach ($raws as $key => $value) {
				$array[] = (array)$value;	
			}
		}

		return $array;
	}

	public static function get_entity_title($id, $type) {
		$entity = entity_load($type, array($id));
		return entity_label($type, array_pop($entity));
	}

	/**
	 * Функция для получения массива параметров по всем типам сущностей.
	 * Которая использует статическую переменную, 
	 * чтобы избежать повторного получения данных из базы данных.
	 * @return array параметры всех типов сущностей.
	 */
	public static function get_entities_info() {
		if (NULL === self::$entities_info) {
			self::$entities_info = entity_get_info();
		}
		return self::$entities_info;
	}

	private static function get_entity_tbl($type) {
		$entities_info = self::get_entities_info();
		if ( isset($entities_info[$type]) ) {
			return $entities_info[$type]['base_table'];
		}
	}

	public function get_available_entities($title_contains = '', $limit = 10) {
		$types_and_bundles = $this->entity_params;
		if ($this->entity_params) {
			foreach ($this->entity_params as $key => $arr) {
				if ( in_array($type, array_keys($queries)) ) continue;

				$type = $arr['type'];
				$tbl = self::get_entity_tbl($type);
				$queries[$type] = db_select($tbl, $tbl);
			}
		}
		$raws = db_select($entities, 't')
			->fields('t');
	}
		
}
