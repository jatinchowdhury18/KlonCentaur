import tensorflow as tf

def pre_emphasis_filter(x, coeff=0.85):
  return tf.concat([x[:, 0:1, :], x[:, 1:, :] - coeff*x[:, :-1, :]], axis=1)

def dc_loss(target_y, predicted_y):
    return tf.reduce_mean(tf.square(tf.reduce_mean(target_y) - tf.reduce_mean(predicted_y))) / tf.reduce_mean(tf.square(target_y))

# Error-to-signal loss
def esr_loss(target_y, predicted_y, emphasis_func=lambda x : x):
    target_yp = emphasis_func(target_y)
    pred_yp = emphasis_func(predicted_y)
    return tf.reduce_sum(tf.square(target_yp - pred_yp)) / tf.reduce_sum(tf.square(target_yp))
