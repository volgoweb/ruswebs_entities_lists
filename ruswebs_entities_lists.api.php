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
 *   rel_add_entity_to_list_checkbox($form, $form['actions']['submit']['#submit'], $list_id, 'my_type_entity');
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
    '#default_values' => !empty($entity_id) ? $list->is_entity_in_list($entity_id) : 0,
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

