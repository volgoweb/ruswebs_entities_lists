<?php

function rel_admin_list_form($form, &$form_state, $list_id = NULL) {
	// получаем объект списка
	$list = new rel_list($list_id);

	$form['#tree'] = TRUE;

	$form['id'] = array(
		'#type'  => 'hidden',
		'#value' => $list->get_id(),
	);

	$form['title'] = array(
		'#type'           => 'textfield',
		'#title'          => t('Title'),
		'#size'           => 20,
		'#maxlength'      => 255,
		'#required'       => TRUE,
		'#default_value' => $list->get_title(),
	);
	
	$form['max_size'] = array(
		'#type'          => 'textfield',
		'#title'         => t('Max entities in list'),
		'#size'          => 20,
		'#maxlength'     => 5,
		'#required'      => TRUE,
		'#default_value' => $list->get_max_size(),
	);

	// выводим поля со всеми типами сущностей (включая все bundle) 
	$form['entity_params'] = array(
		'#type'        => 'fieldset',
		'#title'       => t('Types and bundles of the entity'),
		'#collapsible' => TRUE,
		'#collapsed'   => FALSE,
	);
	$all_types = rel_get_all_types_and_bundles();
	if ($all_types) {
		foreach ($all_types as $i => $arr) {
			$type   = $arr['type'];
			$bundle = $arr['bundle'];

			$form['entity_params'][$i] = array(
				'#type'        => 'fieldset',
				'#title'       => sprintf('Type: "%s", bundle: "%s"', $type, $bundle),
				'#collapsible' => FALSE,
				'#collapsed'   => FALSE,
			);

			$form['entity_params'][$i]['type'] = array(
				'#type'  => 'hidden',
				'#value' => $type,
			);

			$form['entity_params'][$i]['bundle'] = array(
				'#type'  => 'hidden',
				'#value' => $bundle,
			);

			$form['entity_params'][$i]['active'] = array(
				'#type'           => 'checkbox',
				'#title'          => t('Can add to list'),
				'#required'       => FALSE,
				'#default_value' => $list->can_entity_add_to_list($type, $bundle),
			);

			$form['entity_params'][$i]['autoadds'] = array(
				'#type'           => 'checkbox',
				'#title'          => t('Auto adding when entity created.'),
				'#required'       => FALSE,
				'#default_value' => $list->is_entity_autoadd_to_list($type, $bundle),
			);
		}
	}


	$form['actions']['submit'] = array(
		'#type'  => 'submit',
		'#value' => t('Save'),
	);

	return $form;
}

function rel_admin_list_form_submit(&$form, &$form_state) {
	$vs = $form_state['values'];

	$entity_params = array();
	if ($vs['entity_params']) {
		foreach ($vs['entity_params'] as $i => $params) {
			if (empty($params['active']))  continue;

			$type     = $params['type'];
			$bundle   = $params['bundle'];
			$autoadds = $params['autoadds'];
			$entity_params += rel_list::get_array_entity_params($type, $bundle, $autoadds);
		}
	}

	$list_params = array(
		'title' => $vs['title'],
		'max_size' => $vs['max_size'],
		'entity_params' => $entity_params,
	);

	$list = new rel_list();
	$list->set($list_params);
	$list->save();	

	$form_state['redirect'] = 'admin/content/ruswebs-entities-lists/lists';
}

function rel_get_all_types_and_bundles() {
	$types = entity_get_info();
	foreach ($types as $type => $params) {
		if (!empty($params['bundles'])) {
			foreach ($params['bundles'] as $bundle => $budle_params) {
				$return[] = array(
					'type'   => $type,
					'bundle' => $bundle,
				);
			}
		}
		else {
			$return[] = array(
				'type'   => $type,
				'bundle' => $bundle,
			);
		}
	}

	return $return;
}

/**
 * Форма редактирования набора сущностей в определенном списке
 */
function rel_admin_list_entities_form($form, $form_state, $list_id = NULL) {
	$form['entities']['#tree'] = TRUE;

	// получаем список всех сущностей
	$list = new rel_list($list_id);
	$entities = $list->get_entities_array();

	// $form['entities'] = array(
	// 	'#type' => 'fieldset',
	// 	'#title'       => t('Entities in list'),
	// 	// '#collapsible' => FALSE,
	// 	// '#collapsed'   => FALSE,
	// );

	if ($entities) {
		foreach ($entities as $i => $arr) {
			$form['entities'][$i]['entity_id'] = array(
				'#type'          => 'textfield',
				'#default_value' => $arr['entity_id'],
				'#size'          => 5,
				'#maxlength'     => 5,
				'#disabled'      => TRUE,
			);

			$form['entities'][$i]['title'] = array(
				'#type'   => 'markup',
				'#markup' => rel_list::get_entity_title($arr['entity_id'], $arr['entity_type']),
			);

			$form['entities'][$i]['weight'] = array(
				'#type' => 'weight',
				'#default_value' => $arr['weight'],
				'#delta' => 10,
				'#title_display' => 'invisible',
			);
		}
	}

	$form['actions'] = array('#type' => 'actions');
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);

	return $form;
}

/**
 * Submit для формы rel_admin_list_entities_form
 * @TODO вынести код сохранения веса в класс
 */
function rel_admin_list_entities_form_submit($form, &$form_state) {
	if (!empty($form_state['values']['entities'])) {
		$form_values = &$form_state['values']['entities'];
		foreach ($form_values as $key => $value) {
			$row = $form_values[$key];
			$query = db_update(rel_list::ENTITIES_TBL)
				->fields(array('weight' => $row['weight']))
				->condition('entity_id', $row['entity_id'])
				->execute();
		}
	}
}

function theme_rel_admin_list_entities_form($vars) {
	$form = $vars['form'];

	$rows = array();

	foreach (element_children($form['entities']) as $key => $arr) {
		$form['entities'][$key]['weight']['#attributes']['class'] = array('entity-weight');
		
		$rows[] = array(
			'data' => array(
				drupal_render($form['entities'][$key]['entity_id']),
				drupal_render($form['entities'][$key]['title']),
				drupal_render($form['entities'][$key]['weight']),
			),
			'class' => array('draggable'),
		);
	}

	$header = array(
		t('entity_id'),
		t('title'),
		t('weight'),
	);

	$table_id = 'list-entities-table';

	$output = theme('table', array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => array('id' => $table_id)
	));

	$output .= drupal_render_children($form);

	drupal_add_tabledrag($table_id, 'order', 'sibling', 'entity-weight');

	return $output;
}
