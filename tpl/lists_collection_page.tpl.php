<table id="rel-lists">
  <?php for ($i = 0; $i < count($lists); $i++): ?>
    <tr>
        <td><a href="/<?php print sprintf('%s/list/%d', REL_URL_ADMIN_LISTS, $lists[$i]->id); ?>"><?php print $lists[$i]->title; ?></a></td>
    </tr>
  <?php endfor; ?>
</table>
<a class="button add-list" href="/<?php print REL_URL_ADMIN_ADD_LIST; ?>">Добавить список</a>
