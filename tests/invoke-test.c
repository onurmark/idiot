#include <glib.h>
#include <gio/gio.h>

typedef struct {
	GMainLoop *main_loop;
	guint n_remaining;
} WriteData;

static void
write_cb(GObject *source_object,
		GAsyncResult *result,
		gpointer user_data)
{
	WriteData *write_data = user_data;
	GOutputStream *stream = G_OUTPUT_STREAM(source_object);
	GError *error = NULL;
	gssize len;

	len = g_output_stream_write_finish(stream, result, &error);
	if (error != NULL) {
		g_error("Error: %s", error->message);
		g_error_free(error);
	}

	write_data->n_remaining--;

	if (write_data->n_remaining == 0) {
		g_main_loop_quit(write_data->main_loop);
	}

	g_message("done");
}

static void
thread_cb(GTask *task,
		gpointer source_object,
		gpointer task_data,
		GCancellable *cancellable)
{
	GFileOutputStream *output_stream1, *output_stream2;
	GMainContext *worker_context;
	GBytes *data;
	const guint8 *buf;
	gsize len;
	WriteData write_data;

	GFile *file1, *file2;

	file1 = g_file_new_for_path("/tmp/test1");
	file2 = g_file_new_for_path("/tmp/test2");

	output_stream1 = g_file_append_to(file1, G_FILE_CREATE_NONE, NULL, NULL);
	output_stream2 = g_file_append_to(file2, G_FILE_CREATE_NONE, NULL, NULL);

	worker_context = g_main_context_new();
	g_main_context_push_thread_default(worker_context);

	/* Set up writes. */
	write_data.n_remaining = 2;
	write_data.main_loop = g_main_loop_new(worker_context, FALSE);

	data = g_task_get_task_data(task);
	buf = g_bytes_get_data(data, &len);

	g_output_stream_write_async(G_OUTPUT_STREAM(output_stream1), buf, len,
			G_PRIORITY_DEFAULT, NULL, write_cb,
			&write_data);
	g_output_stream_write_async(G_OUTPUT_STREAM(output_stream2), buf, len,
			G_PRIORITY_DEFAULT, NULL, write_cb,
			&write_data);

	g_main_loop_run(write_data.main_loop);
	g_task_return_boolean(task, TRUE);

	g_main_loop_unref(write_data.main_loop);

	g_object_unref(output_stream1);
	g_object_unref(output_stream2);

	g_object_unref(file1);
	g_object_unref(file2);

	g_main_context_pop_thread_default(worker_context);
	g_main_context_unref(worker_context);
}

static void
parallel_writes_async(GBytes *data,
		GMainContext *interesting_context,
		GCancellable *cancellable,
		GAsyncReadyCallback callback,
		gpointer user_data)
{
	GTask *task;

	g_main_context_push_thread_default(interesting_context);

	task = g_task_new(NULL, cancellable, callback, user_data);
	g_task_set_task_data(task, data, (GDestroyNotify)g_bytes_unref);
	g_task_run_in_thread(task, thread_cb);
	g_object_unref(task);

	g_main_context_pop_thread_default(interesting_context);
}

static void mycallback(GObject *source_object,
		GAsyncResult *result,
		gpointer user_data)
{
	GMainLoop *main_loop = user_data;
	GError *error = NULL;
	gboolean res;

	g_return_if_fail(g_task_is_valid(result, source_object));

	res = g_task_propagate_boolean(G_TASK(result), &error);
	
	g_message("Result %s", res ? "Success" : "Failure");

	g_main_loop_quit(main_loop);
}

int main(int argc, char *argv[])
{
	GMainLoop *loop;
	GSource *source;
	GBytes *bytes;

	gchar *data = "Lorem ipsum dolor sit amet, officia excepteur ex fugiat reprehenderit enim labore culpa sint ad nisi Lorem pariatur mollit ex esse exercitation amet. Nisi anim cupidatat excepteur officia. Reprehenderit nostrud nostrud ipsum Lorem est aliquip amet voluptate voluptate dolor minim nulla est proident. Nostrud officia pariatur ut officia. Sit irure elit esse ea nulla sunt ex occaecat reprehenderit commodo officia dolor Lorem duis laboris cupidatat officia voluptate. Culpa proident adipisicing id nulla nisi laboris ex in Lorem sunt duis officia eiusmod. Aliqua reprehenderit commodo ex non excepteur duis sunt velit enim. Voluptate laboris sint cupidatat ullamco ut ea consectetur et est culpa et culpa duis.";


	bytes = g_bytes_new(data, strlen(data));

	loop = g_main_loop_new(NULL, FALSE);

	parallel_writes_async(bytes, g_main_context_default(), NULL, (GAsyncReadyCallback)mycallback, loop);

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	return EXIT_SUCCESS;
}
