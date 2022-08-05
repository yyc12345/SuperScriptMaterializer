// tab switcher
$(document).ready(function() {
    // References
    // https://developer.mozilla.org/en-US/docs/Web/Accessibility/ARIA/Roles/Tab_Role
    $('[role="tab"]').click(function() {
        $('[aria-selected="true"]').attr('aria-selected', false);
        $(this).attr('aria-selected', true);
        $('[role="tabpanel"]').attr('hidden', true);
        $(`#${$(this).attr('aria-controls')}`).removeAttr('hidden');
    });
});

