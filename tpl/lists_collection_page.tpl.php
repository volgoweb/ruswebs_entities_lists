<table id="rel-lists">
	<tr>
		<?php for ($i = 0; $i < count($lists); $i++): ?>
			<td><a href="/<?php print sprintf('%s/list/%d/params', REL_URL_ADMIN, $lists[$i]->id); ?>"><?php print $lists[$i]->title; ?></a></td>
		<?php endfor; ?>
	</tr>
</table>
<a class="button add-list" href="/<?php print REL_URL_ADMIN_ADD_LIST; ?>">Добавить список</a>
