<?php
/*
 * API функции, используемые для использования функционала модуля в сторонних модулях.
 */

/**
 * Функция служит для добавления в форму создания/редактирования сущности элемента типа "чекбокс" 
 * для управления присутствия данной сущности в определенном списке.
 *
 * Пример использования:
 *
 * function my_module_form_MY_TYPE_ENTITY_UI_EDIT_FORM_alter(&$form, $form_state) {
 *   $list_id = 1;
 *   $entity_id = NULL; // в случае, если форма редактирования существующей сущности, то подставляйте реальное id сущности
 *   $weight = 100; // вес галочки "включить в список" в форме
 *   rel_add_entity_to_list_checkbox($form, $form['actions']['submit']['#submit'], $list_id, 'my_type_entity', $entity_id, $weight);
 * }
 *
 *
 * @param array $form - массив формы (в стандарте form api), в которую надо добавить чекбокс-элемент добавления/удаления из определенного списка
 * @param array $submit_array - массив submit-функций формы (обычно содержится в $form['actions']['submit']['#submit'])
 * @param int $list_id - идентификатор списка, в который будет добавляться сущность
 * @param string $entity_type - тип сущности, в форму редактирования которой добавляется элемент
 * @param int $entity_id - идентификатор сущности на случай, если рассматривается форма редактирования сущности
 * @param int $weight - вес вставляемого элемента формы (его позиционирование относительно других элементов формы при отображении)
 */
function rel_add_entity_to_list_checkbox(&$form, &$submit_array, $list_id, $entity_type, $entity_id = NULL, $weight = 1000) {
  $list = new rel_list($list_id);

  $list_title = $list->get_title();

  $name = "rel_entity_to_list_$list_id";
  $form[$name] = array(
    '#type' => 'checkbox',
    '#title' => t('Add to list "@title"', array('@title' => $list_title)),
    '#default_value' => !empty($entity_id) ? $list->is_entity_in_list($entity_id, $entity_type) : 0,
    '#weight' => $weight,
  );

  if (!isset($form['rel_entity_type'])) {
    $form['rel_entity_type'] = array(
      '#type' => 'hidden',
      '#value' => $entity_type,
    );
  }

  $submit_array[] = 'rel_add_entity_to_list_submit';
}

/**
 * @param int $entity_id - идентификатор сущности
 * @param string $type - тип сущности
 * Возвращает массив всех списков, в которую можно добавить сущность с указанным id. 
 */
// rel_get_allowed_lists($entity_id, $type)

/**
 * @param int $entity_id - идентификатор сущности
 * @param string $type - тип сущности
 * Возвращает массив объектов списков, в которых присутствует сущность с указанным идентификатором и типом.
 */
// rel_get_lists_with_entity($entity_id, $type) 

/**
 * @param int $entity_id - идентификатор сущности
 * @param string $type - тип сущности
 * @param int $list_id - идентификатор списка
 * Возвращает TRUE, если сущность содержится в списке
 */
// rel_is_list_contains_entity($id, $type, $list_id)

/**
 * Функция возвращает массив сущностей, содержащихся в списке.
 *
 * @param int $list_id - идентификатор списка сущностей
 * @return array
 * Пример возвращаемого массива:
    array (
      0 => array (
        'list_id' => '2',
        'entity_id' => '210002',
        'entity_type' => 'sobesednik_content',
        'weight' => '-10',
      ),
      1 => array (
        'list_id' => '2',
        'entity_id' => '192562',
        'entity_type' => 'sobesednik_content',
        'weight' => '-10',
      ),
    )

 */
function rel_get_list_entities($list_id) {
  $list = new rel_list($list_id);
  return $list->get_entities_array();
}
