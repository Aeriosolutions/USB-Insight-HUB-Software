/* import type { PageLoad } from './$types';
import { redirect } from '@sveltejs/kit';

export const load = (async () => {
    throw redirect(302, 'routes/+page.svelte'); // Redirect to '/new-page'
}) satisfies PageLoad; */

import type { PageLoad } from './$types';

export const load = (async ({ fetch }) => {
    return {
        title: 'Control Panel'
    };
}) satisfies PageLoad;