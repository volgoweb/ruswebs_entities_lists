<?php
	
/**
 * Класс, описывающий список сущностей
 */
class rel_list {
	/**
	 * идентификатор списка
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

	public function __construct($id = NULL) {
		if (!empty($id)) {
			$this->id = (int)$id;
			$result = $this->load();
			if (!$result) $this->id = NULL;
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

		if (empty($raw)) return FALSE;

		foreach (array('title', 'max_size') as $name) {
			$this->$name = $raw->$name;
		}

		$this->entity_params = unserialize($raw->entity_params);

		return TRUE;
	}

	/**
	 * @return int идентификатор списка
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

		$this->id = db_insert(self::LIST_TBL)
			->fields($fields)
			->execute();
	}

	public function remove() {
		db_delete(self::LIST_TBL)
			->condition('id', $this->id)
			->execute();
		$this->remove_all_entites();
	}

	private function remove_all_entites() {
		db_delete(self::ENTITIES_TBL)
			->condition('list_id', $this->id)	
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
			return $entities_info[$type]['base table'];
		}
	}

	public static function get_entity_id_colname($type) {
		$entities_info = self::get_entities_info();
		if ( isset($entities_info[$type]) ) {
			return $entities_info[$type]['entity keys']['id'];
		}
	}

	public static function get_entity_title_colname($type) {
		$entities_info = self::get_entities_info();
		if ( isset($entities_info[$type]) ) {
			return $entities_info[$type]['entity keys']['label'];
		}
	}

	/**
	 * Функция служит для получения списка основных параметров сущностей 
	 * всех разрешенных для данного списка типов, содержащих в заголовке текст $title_contains
	 * Возвращаемый массив ограничен количеством сущностей, указанном в аргументе $limit
	 * @param $title_contains string
	 * @param $limit int
	 * @return array
	 */
	public function get_available_entities($title_contains = '', $limit = 10) {
		if (empty($this->entity_params)) return;

		// по всем типам сущностей, которые можно добавлять в список
		// получаем набор строк из их таблиц
		$queries =  array();
		foreach ($this->entity_params as $key => $arr) {
			// В $this->entity_params массив содержит столько элементов,
			// сколько есть комбинаций типа и bundle сущности. 
			// Сущности всех bundle, но одного типа, находятся в одной таблице,
			// поэтому избегаем повторного формирования запроса для одного типа
			$type = $arr['type'];
			if ( array_key_exists($type, $queries) ) continue;

			// так как каждый тип сущности хранится в разных таблицах и 
			// в разных колонках хранит идентификатор и заголовок сущности, то определяем их
			$tbl       = self::get_entity_tbl($type);
			$id_col    = self::get_entity_id_colname($type);
			$title_col = self::get_entity_title_colname($type);

			if (empty($id_col)) continue;

			// можно использовать EntityFieldQuery(), но это почти не дает преимуществ
			// Так как имена колонок в разных таблицах различно, то добавим одинаковые алиасы для полей
			$q = db_select($tbl, $tbl);
			$q->addField($tbl, $id_col, 'id');
			$q->addField($tbl, $title_col, 'title');
			$q->addExpression(sprintf(" '%s' ", $type), 'type');
			// @TODO добавить проверку "нет ли уже в списке данной сущности?"

			if (!empty($title_col) && !empty($title_contains)) {
				$cond_col = $tbl . '.' . $title_col;
				$cond_value = '%' . db_like($title_contains) . '%';
				$q->condition($cond_col, $cond_value, 'LIKE');
			}

			$queries[$type] = $q;
		}

		// Объединяем запросы в union, так как нам надо получить список сущностей всех типов
		$q1 = array_shift($queries);
		if ($queries) {
			foreach ($queries as $key => $q) {
				$q1->union($q, 'UNION ALL');
			}
		}

		$raw = $q1
			->range(0, $limit)
			->execute()
			->fetchAll();
		
		return $raw;
	}

	/**
	 * Возвращает TRUE, если указанная сущность уже присутствует в списке
	 * @param int $id - идентификатор сущности
	 * @param string $type - тип сущности
	 */ 
	public function list_contains_entity($id, $type) {
		$query = db_select(self::ENTITIES_TBL);
		$query->addExpression('count(entity_id)', 'entities_count');
		$query->condition('list_id', $this->id);
		$query->condition('entity_type', $type);
		$query->condition('entity_id', $id);
		$count = $query->execute()->fetchField();
		if (!empty($count)) return TRUE;
		return FALSE;
	}

	/**
	 * @return int минимальный вес среди сущностей данного списка
	 */
	public function get_min_weight() {
		$query = db_select(self::ENTITIES_TBL);
		$query->addExpression('MIN(weight)', 'min_weight');
		$query->condition('list_id', $this->id);
		$min_weight = $query->execute()->fetchField();

		return (int)$min_weight;
	}

	/**
	 * Добавляет сущность в данный список
	 * @param int $id - идентификатор сущности
	 * @param string $type - тип сущности
	 */ 
	public function add_entity($id, $type) {
		if ($this->list_contains_entity($id, $type))  return;

		$weight = $this->get_min_weight() - 10;
		$fields = array(
			'list_id' => $this->id,
			'entity_id' => $id,
			'entity_type' => $type,
			'weight' => $weight,
		);
		db_insert(self::ENTITIES_TBL)
			->fields($fields)
			->execute();
	}

	/**
	 * Удаляет сущность из данного списка 
	 * @param int $id - идентификатор сущности
	 * @param string $type - тип сущности
	 */ 
	public function remove_entity($id, $type) {
		db_delete(self::ENTITIES_TBL)
			->condition('entity_id', $id)
			->condition('entity_type', $type)
			->execute();
	}
}
