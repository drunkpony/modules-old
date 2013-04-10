#include <math.h>
#include <fast_math.h>
#include <holonomic/trajectory_manager_utils.h>
#include <scheduler.h>
#include <quadramp.h>
#include <ramp.h>

#define RAD 10

/** @todo : use manage */
void holonomic_trajectory_manager_event(void * param)
{
    ///@todo : probablement des fonctions de la lib math qui font ça
    struct h_trajectory *traj = (struct h_trajectory *) param;
    double x = holonomic_position_get_x_double(traj->position);
    double y = holonomic_position_get_y_double(traj->position);
    int32_t s_consign = 0;  /**< The speed consign */
    int32_t a_consign = 0;  /**< The angle consign */
    int32_t o_consign = 0;  /**< The angular speed (omega) consign */
    
    float target_norm =  sqrtf(pow(traj->xy_target.x,2)+pow(traj->xy_target.y,2));
    float position_norm = sqrtf(pow(x,2)+pow(y,2));
    int32_t distance2target = sqrtf(pow(x - traj->xy_target.x,2) + pow(y - traj->xy_target.y,2));

    /* step 1 : process new commands to quadramps */
    switch (traj->moving_state) 
    {
         case MOVING_STRAIGHT:
            /* Calcul de la consigne d'angle */
            //a_consign = acosf((traj->xy_target.x*x + traj->xy_target.y*y)/
            //(target_norm * position_norm));
            //a_consign = cs_do_process(traj->csm_angle, a_consign);
            ///* Calcul de la consigne de vitesse */
            //s_consign = cs_do_process(traj->csm_speed, distance2target);
            cs_do_process(traj->csm_speed,10);
            cs_do_process(traj->csm_angle,0);
            break;
         case MOVING_CIRCLE:
            a_consign = M_PI_2 ;//- 
            /** @todo: please check if used correctly. Need to pass a radius. */
            s_consign = cs_do_process(traj->csm_speed, length_arc_of_circle_p(traj, RAD));
            break;
        case MOVING_IDLE:
            break;
    }
    /* step 2 : check the end of the move */
    //if (holonomic_robot_in_xy_window(traj, traj->d_win) ||
        //holonomic_robot_in_angle_window(traj, traj->a_win))
            //holonomic_delete_event(traj);
}

/** near the target (dist in x,y) ? */
uint8_t holonomic_robot_in_xy_window(struct h_trajectory *traj, double d_win)
{
    vect2_cart vcp = {.x = holonomic_position_get_x_double(traj->position),
                      .y = holonomic_position_get_y_double(traj->position)};
    return (vect2_dist_cart(&vcp, &traj->xy_target) < d_win);
    
}

/** returns true if the robot is in an area enclosed by a certain angle */
uint8_t holonomic_robot_in_angle_window(struct h_trajectory *traj, double a_win_rad)
{
    double d_a =  traj->a_target - holonomic_position_get_a_rad_double(traj->position);

    if (ABS(d_a) < M_PI) {
        return (ABS(d_a) < (a_win_rad/2));
    } else {
        return ((M_PI_2-ABS(d_a)) < (a_win_rad/2));
    }
}

/** remove event if any @todo */
void holonomic_delete_event(struct h_trajectory *traj)
{
    //TODO : delete very soon! this is only to eliminate warnings!
    (void)traj;

    //set_quadramp_speed(traj, traj->d_speed, traj->a_speed);
    //set_quadramp_acc(traj, traj->d_acc, traj->a_acc);
    //if ( traj->scheduler_task != -1) {
        //DEBUG(E_TRAJECTORY, "Delete event");
        //scheduler_del_event(traj->scheduler_task);
        //traj->scheduler_task = -1;
    //}
}

/** schedule the trajectory event */
void holonomic_schedule_event(struct h_trajectory *traj)
{
    if ( traj->scheduler_task != -1) {
        DEBUG(E_TRAJECTORY, "Schedule event, already scheduled");
    }
    else {
        traj->scheduler_task =
            scheduler_add_periodical_event_priority(&holonomic_trajectory_manager_event,
                                (void*)traj,
                                TRAJ_EVT_PERIOD, 30);
    }
}

/** do a modulo 2.pi -> [-Pi,+Pi], knowing that 'a' is in [-3Pi,+3Pi] */
double holonomic_simple_modulo_2pi(double a)
{
    if (a < -M_PI) {
        a += M_PI_2;
    }
    else if (a > M_PI) {
        a -= M_PI_2;
    }
    return a;
}

/** do a modulo 2.pi -> [-Pi,+Pi] */
double holonomic_modulo_2pi(double a)
{
    double res = a - (((int32_t) (a/M_PI_2)) * M_PI_2);
    return holonomic_simple_modulo_2pi(res);
}

/** calculates the lenght of an arc of a circle given an end point and a radius */
float length_arc_of_circle_p(struct h_trajectory *traj, float rad)
{
    vect2_cart vcp = {.x = holonomic_position_get_x_double(traj->position),
                      .y = holonomic_position_get_y_double(traj->position)};
    float d_r = vect2_dist_cart(&vcp, &traj->xy_target) / rad;

    /* law of cosines */
    return (rad * fast_acosf(1 - 0.5 * d_r * d_r));
}
