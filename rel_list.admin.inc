<?php

function rel_admin_list_form($form, &$form_state, $list_id = NULL) {
	$list_exists = FALSE;
	if (!empty($list_id)) {
		// получаем объект списка
		$list = new rel_list($list_id);
		// проверяем существует ли список с указанным идентификатором
		if ($list->get_id() == NULL) {
			$list_exists = FALSE;
		}
		else {
			$list_exists = TRUE;
		}

		if (!$list_exists) {
			$form['message'] = array(
				'#type' => 'markup',
				'#markup' => t("List with id=$list_id not found!"),
			);
			return $form;
		}
	}

	$form['#tree'] = TRUE;

	$form['id'] = array(
		'#type'  => 'hidden',
		'#value' => $list_exists ? $list->get_id() : '',
	);

	$form['title'] = array(
		'#type'           => 'textfield',
		'#title'          => t('Title'),
		'#size'           => 20,
		'#maxlength'      => 255,
		'#required'       => TRUE,
		'#default_value' => $list_exists ? $list->get_title() : '',
	);
	
	$form['max_size'] = array(
		'#type'          => 'textfield',
		'#title'         => t('Max entities in list'),
		'#size'          => 20,
		'#maxlength'     => 5,
		'#required'      => TRUE,
		'#default_value' => $list_exists ? $list->get_max_size() : '',
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
				'#default_value' => $list_exists ? $list->can_entity_add_to_list($type, $bundle) : '',
			);

			$form['entity_params'][$i]['autoadds'] = array(
				'#type'           => 'checkbox',
				'#title'          => t('Auto adding when entity created.'),
				'#required'       => FALSE,
				'#default_value' => $list_exists ? $list->is_entity_autoadd_to_list($type, $bundle) : '',
			);
		}
	}


	$form['actions']['submit'] = array(
		'#type'  => 'submit',
		'#value' => t('Save'),
	);

	$form['actions']['remove'] = array(
		'#type'  => 'submit',
		'#value' => t('Remove'),
		'#submit'  => array('rel_list_remove'),
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

	$form_state['redirect'] = sprintf('%s/list/%d/entities', REL_URL_ADMIN_LISTS, $list->get_id());
}

function rel_list_remove($form, &$form_state) {
	$list_id = $form_state['values']['id'];

	$list = new rel_list($list_id);
	$list->remove();
	drupal_set_message(t("List '@title' has been remove.", array('@title' => $list->get_title())));
	drupal_goto('/' . REL_URL_ADMIN_LISTS);
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

	$form['list_id'] = array(
		'#type' => 'hidden',
		'#value' => $list_id,
	);

	$form['add']['#tree'] = TRUE;

	$form['add']['title'] = array(
		'#type' => 'textfield',
		'#title'       => t('Title contains'),
		'#autocomplete_path' => 'ruswebs-entities-lists/get-available-entities/' . $list_id,
	);

	$form['add']['entity_add_submit'] = array(
		'#type'   => 'submit',
		'#value'  => t('Add'),
		'#submit' => array('rel_add_entity_to_list'),
	);

	if ($entities) {
		foreach ($entities as $i => $arr) {
			$form['entities'][$i]['entity_id'] = array(
				'#type'          => 'textfield',
				'#default_value' => $arr['entity_id'],
				'#size'          => 5,
				'#maxlength'     => 5,
				'#disabled'      => TRUE,
			);

			$form['entities'][$i]['entity_type'] = array(
				'#type'          => 'textfield',
				'#default_value' => $arr['entity_type'],
				'#size'          => 20,
				'#maxlength'     => 20,
				'#disabled'      => TRUE,
			);

			$form['entities'][$i]['bundle'] = array(
				'#type'          => 'textfield',
				'#default_value' => rel_list::get_entity_bundle($arr['entity_id'], $arr['entity_type']),
				'#size'          => 20,
				'#maxlength'     => 20,
				'#disabled'      => TRUE,
			);

			$form['entities'][$i]['title'] = array(
				'#type'   => 'markup',
				'#markup' => rel_list::get_entity_title($arr['entity_id'], $arr['entity_type']),
			);

			$form['entities'][$i]['remove'] = array(
				'#type'   => 'submit',
				'#value'  => t('Remove'),
				'#name'   => "remove_$i",
				'#submit' => array('rel_remove_entity_from_list'),
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
		// сохраняем новые веса сущностей в списке
		$form_values = &$form_state['values']['entities'];
		foreach ($form_values as $key => $value) {
			$row = $form_values[$key];
			$query = db_update(rel_list::ENTITIES_TBL)
				->fields(array('weight' => $row['weight']))
				->condition('entity_id', $row['entity_id'])
				->condition('entity_type', $row['entity_type'])
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
				drupal_render($form['entities'][$key]['entity_type']),
				drupal_render($form['entities'][$key]['bundle']),
				drupal_render($form['entities'][$key]['remove']),
				drupal_render($form['entities'][$key]['weight']),
			),
			'class' => array('draggable'),
		);
	}

	$header = array(
		t('entity_id'),
		t('title'),
		t('entity_type'),
		t('bundle'),
		t('remove'),
		t('weight'),
	);

	$table_id = 'list-entities-table';

	$output = drupal_render($form['add']);

	$output .= theme('table', array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => array('id' => $table_id)
	));

	$output .= drupal_render_children($form);

	drupal_add_tabledrag($table_id, 'order', 'sibling', 'entity-weight');

	return $output;
}

/**
 * Callback функция, возвращающая значения для автодополняемого поля добавления сущности в список
 * @return string (json) 
 */
function rel_get_available_entities($list_id, $text) {
	if (empty($list_id) || !is_numeric($list_id)) return;

	$list = new rel_list($list_id);
	$count = 10;
	$raws = $list->get_available_entities($text, $count);
	$return = array();
	if ($raws) {
		foreach ($raws as $i => $obj) {
			$key = rel_union_entity_params_for_textfield($obj->id, $obj->type, $obj->title);
			$return[$key] = $key;
		}
	}

	return drupal_json_output($return);
}

function rel_add_entity_to_list($form, &$form_state) {
	$list_id = $form_state['values']['list_id'];
	$list = new rel_list($list_id);

	$title_value = $form_state['values']['add']['title'];
	list($entity_id, $entity_type) = rel_extract_id_and_type($title_value);

	$list->add_entity($entity_id, $entity_type);
}

function rel_union_entity_params_for_textfield($id, $type, $title) {
	return sprintf('%s [%d:%s]', $title, $id, $type);
}

function rel_extract_id_and_type($string) {
	preg_match('/\[(\d+):([^\]]+)\]/', $string, $match);
	array_shift($match);
	return $match;
}

function rel_remove_entity_from_list($form, &$form_state) {
	$list_id = $form_state['values']['list_id'];

	$list = new rel_list($list_id);

	$button_name = $form_state['clicked_button']['#name'];
	$row_key     = str_replace('remove_', '', $button_name);
	$entity_id   = $form_state['values']['entities'][$row_key]['entity_id'];
	$entity_type = $form_state['values']['entities'][$row_key]['entity_type'];

	$list->remove_entity($entity_id, $entity_type);
}

