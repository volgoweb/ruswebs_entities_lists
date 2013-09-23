<?php

function rel_admin_list_form($form, &$form_state, $list = NULL) {
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
		'#type'           => 'textfield',
		'#title'          => t('Max entities in list'),
		'#size'           => 20,
		'#maxlength'      => 255,
		'#required'       => TRUE,
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

function rel_list_edit_page($list_id) {
	// получаем объект списка
	$list = new rel_list($list_id);
	// формируем начальные значения для формы
	// выводим форму
	$form = drupal_get_form('rel_admin_list_form', $list);
	return drupal_render($form);
}

function rel_list_add_page() {
	// выводим пустую форму
	$form = drupal_get_form('rel_admin_list_form');
	return drupal_render($form);
}

/**
 * Форма редактирования набора сущностей в определенном списке
 */
function rel_admin_list_entities_form($form, $list_id) {
	$form['#tree'] = TRUE;

	// получаем список всех сущностей
	$list = new rel_list($list_id);
	$entities = $list->get_entities_array();

	$form['entities'] = array(
		'#type' => 'fieldset',
		'#title'       => t('Entities in list'),
		// '#collapsible' => FALSE,
		// '#collapsed'   => FALSE,
	);

	if ($entities) {
		foreach ($entities as $i => $arr) {
			$form['entities'][$i]['id'] = array(
				'#type' => 'markup',
				'#markup' => $arr['id'],
			);

			$form['entities'][$i]['title'] = array(
				'#type' => 'markup',
				'#markup' => rel_list::get_entity_title($arr['id']),
			);

			$form['entities'][$i]['weight'] = array(
				'#type' => 'markup',
				'#markup' => $arr['weight'],
			);
		}
	}

	// $form['add'] = 
}

