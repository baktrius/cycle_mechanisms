import { reactive } from "vue";

export function createOverview(id, args, xs, ys, ids) {
    return reactive({
        type: "overview",
        id,
        args,
        xs,
        ys,
        ids,
    });
}