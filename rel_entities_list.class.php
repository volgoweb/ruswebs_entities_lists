<?php
	
/**
 * Класс, описывающий список сущностей
 */
class rel_entities_list {
	protected $max_size = 5;
	protected $types    = array();
	protected $autoadds = FALSE;
	protected $title_field;
	protected $entites  = array();

	static function get_schema() {
		$schema = array();

		$schema['rel_entity_types'] = array(
			'description' => 'Relationship list and array of the entity type. Module ruswebs_entities_lists.',
			'fields' => array(
				'id' => array(
					'type'     => 'serial',
					'unsigned' => TRUE,
					'not null' => TRUE,
				),
				'entity_type' => array(
					'type'     => 'varchar',
					'length'   => 254,
					'not null' => TRUE,
				),
				'list_id' => array(
					'type'     => 'int',
					'unsigned' => TRUE,
					'not null' => TRUE,
				),
			),
			'indexes' => array(
				'list_id' => array('list_id'),
			),
			'primary key' => array('id'),
		);

		$schema['rel_entities'] = array(
			'description' => 'Relationship list and array of the entities. Module ruswebs_entities_lists.',
			'fields' => array(
				'id' => array(
					'type'     => 'serial',
					'unsigned' => TRUE,
					'not null' => TRUE,
				),
				'entity_id' => array(
					'type'     => 'int',
					'unsigned' => TRUE,
					'not null' => TRUE,
				),
				'list_id' => array(
					'type'     => 'int',
					'unsigned' => TRUE,
					'not null' => TRUE,
				),
			),
			'indexes' => array(
				'list_id' => array('list_id'),
			),
			'primary key' => array('id'),
		);
	
		return $schema;
	}
}
